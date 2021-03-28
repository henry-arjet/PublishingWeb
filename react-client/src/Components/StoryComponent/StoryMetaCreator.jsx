import React, { useState, useContext, useEffect } from 'react';
import { AuthContext } from "../Context/Auth";
import { useFormik } from 'formik';
import { Form, Button, Container, Col, Row } from 'react-bootstrap';
import { Redirect } from 'react-router';

//This component creates the metadata for a story and puts it in the database. 
//This must happen before the actual html content is submitted.
//After this function succeeds, a new story will be in the database, but it will have no html content associated with it

function StoryMetaCreator(){
    const [head, setHead] = useState({style:{}, text: "Let's fill in some basics for your new story"});
    const [shouldRedirect, setShouldRedirect] = useState(0);
    const [cats, setCats] = useState(0);
    const [catDict, setCatDict] = useState(0);
    let auth = useContext(AuthContext);

    useEffect(() => {
        fetch(window.location.protocol + "//" + window.location.host + '/cats.json')
        .then(response => response.json())
        .then(data => {          
            let categories = [<option>N/A</option>]; //the list of categories that is displayed as options
            let categoryDict = {"N/A": 0}; //how we convert those options to a bitmask
            data.forEach(element => {
                categories = categories.concat(<option>{element.name}</option>);
                categoryDict[element.name] = element.mask;
            });
            console.log(categoryDict);
            setCatDict(categoryDict);
            setCats(categories);
        });
    }, []);

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
        let mask = 0;
        mask |= catDict[values.cats1];
        mask |= catDict[values.cats2];
        mask |= catDict[values.cats3];
        fetch(window.location.pathname, { //Try and add to the database
            method: "PUT",
            headers: {'Content-Type': 'application/json', Authorization: auth.authTokens.head},
            body: JSON.stringify({title: values.title, cats: mask})
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
                console.log("something went wrong :{"); //Note: ':{' is not part of the code, it's a face.
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
                <Form.Row>
                    <Form.Group controlId="cats1" as={Col}>
                        <Form.Label>Category One</Form.Label>
                        <Form.Control as="select" required
                        value={formik.values.cats1}
                        onChange={formik.handleChange}
                        onBlur={formik.handleBlur}
                        isInvalid={formik.touched.cats1 && formik.errors.cats1 }>
                            {cats}
                        </Form.Control>
                        <Form.Control.Feedback type="invalid" >
                            {formik.errors.cats1}
                        </Form.Control.Feedback>
                    </Form.Group>
                    <Form.Group controlId="cats2" as={Col}>
                        <Form.Label>Category Two</Form.Label>
                        <Form.Control as="select" required
                        value={formik.values.cats2}
                        onChange={formik.handleChange}
                        onBlur={formik.handleBlur}
                        isInvalid={formik.touched.cats2 && formik.errors.cats2 }>
                            {cats}
                        </Form.Control>
                        <Form.Control.Feedback type="invalid" >
                            {formik.errors.cats2}
                        </Form.Control.Feedback>
                    </Form.Group>
                    <Form.Group controlId="cats3" as={Col}>
                        <Form.Label>Category Three</Form.Label>
                        <Form.Control as="select" required
                        value={formik.values.cats3}
                        onChange={formik.handleChange}
                        onBlur={formik.handleBlur}
                        isInvalid={formik.touched.cats3 && formik.errors.cats3 }>
                            {cats}
                        </Form.Control>
                        <Form.Control.Feedback type="invalid" >
                            {formik.errors.cats3}
                        </Form.Control.Feedback>
                    </Form.Group>
                </Form.Row>
                <Button variant="dark" type="submit">Let's Do This!</Button>
            </Form>
        </Container>
    )
}
  
  export default StoryMetaCreator;