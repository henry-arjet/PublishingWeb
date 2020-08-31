import React, { useState, useContext } from 'react';
import { AuthContext } from "../Context/Auth";
import { useFormik } from 'formik';
import { Form, Button, Container } from 'react-bootstrap';


function StoryMetaCreator(){
    const [head, setHead] = useState({style:{}, text: "Let's fill in some basics for your new story"});
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
            errors.title = 'Must be 40 characters or less';
        }
        return errors;
    }
    function handleSubmit(values){
        fetch(window.location.pathname, {
            method: "PUT",
            headers: {'Content-Type': 'application/json', 'Authorization': "Basic " + btoa(auth.authTokens.id + ":" + auth.authTokens.password),},
            body: JSON.stringify({title: values.title,})
        }).then(result => {
            if (result.status === 201) {
              console.log("success!")
            } else if (result.status == 401) {
                setHead({style: {color:"red"},text: "Please log in"});
            } else if (result.status==403){
                setHead({style: {color:"red"}, text: "I'm sorry " + auth.authTokens.username + ", I'm afraid I can't let you do that"})
            }
            else{
                setHead({styles: {color:"red"},text: "Something went wrong, but I don't know what"});
                console.log("something went wrong :{");
            }
        });
    }
    return (
        <Container className="page">
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