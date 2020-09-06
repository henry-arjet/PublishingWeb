import React, { useState, useContext } from 'react';
import { AuthContext } from "../Context/Auth";
import { useFormik } from 'formik';
import { Form, Button, Container } from 'react-bootstrap';
import { Redirect } from 'react-router';

//This component creates the metadata for a story and puts it in the database. 
//This must happen before the actual html content is submitted.
//After this function succeeds, a new story will be in the database, but it will have no html content associated with it

function StoryMetaCreator(){
    const [head, setHead] = useState({style:{}, text: "Let's fill in some basics for your new story"});
    const [shouldRedirect, setShouldRedirect] = useState(0);
    let auth = useContext(AuthContext);
    const formik = useFormik({
        initialValues: {
          Title: '',
        },
        validate,
        onSubmit: values => {handleSubmit(values);},
    });
    function validate(values){
        const errors = {};
        if(!values.title){
            errors.title = "Required";
        } else if (values.title.length > 40) {
            errors.title = 'Must be 40 characters or less'; //Actual db limit is 255, but I don't want to have to deal with super long titles
        }
        return errors;
    }
    function handleSubmit(values){
        fetch(window.location.pathname, { //Try and add to the database
            method: "PUT",
            headers: {'Content-Type': 'application/json', 'Authorization': "Basic " + btoa(auth.authTokens.id + ":" + auth.authTokens.password),},
            body: JSON.stringify({title: values.title,})
        }).then(result => {
            if (result.status === 201) { //success. Added
              result.text().then(id => setShouldRedirect(id));
            } else if (result.status == 401) {
                setHead({style: {color:"red"},text: "Please log in"});
            } else if (result.status==403){
                setHead({style: {color:"red"}, text: "I'm sorry " + auth.authTokens.username + ", I'm afraid I can't let you do that"})
            }
            else{
                setHead({styles: {color:"red"},text: "Something went wrong, but I don't know what"});
                console.log("something went wrong :{");//Note: ':{' is not part of the code, it's a face.
            }
        });
    }

    function testRedirect(){
        if (shouldRedirect != 0){
            return(
                <Redirect to= {"/writer/" + shouldRedirect}/>
            );
        }
    }
    
    return (
        <Container className="page">
            {testRedirect()}
            <Form noValidate onSubmit={formik.handleSubmit} classname="signupForm">
                <h2 style={head.style}>{head.text}</h2>
                <Form.Group controlId="title">
                    <Form.Label>Title</Form.Label>
                    <Form.Control required type="text" placeholder="Title"
                    value={formik.values.title}
                    onChange={formik.handleChange}
                    onBlur={formik.handleBlur}
                    isInvalid={formik.touched.title && formik.errors.title }/>
                    <Form.Control.Feedback type="invalid" >
                        {formik.errors.title}
                    </Form.Control.Feedback>
                </Form.Group>
                <Button variant="dark" type="submit">Let's Do This!</Button>
            </Form>
        </Container>
    )
}
  
  export default StoryMetaCreator;