import React, {Component, useState } from 'react';
import Button from 'react-bootstrap/Button';
import { Container, Row, Form, Col } from 'react-bootstrap';
import { useFormik } from 'formik';
import { useAuth } from "../Context/Auth";

function SignUpForm(){
    const [isError, setIsError] = useState(false);
    const formik = useFormik({
        initialValues: {
          username: '',
          password: '',
          password2: '',
        },
        validate,
        onSubmit: values => {handleSubmit(values);},
    });
    function validate(values){
        const errors = {};
        if(!values.username){
            errors.username = "Required";
        } else if (values.username.length > 40) {
            errors.username = 'Must be 40 characters or less';
        }
        if(!values.password){
            errors.password = "Required";
        }
        if(!values.password2){
            errors.password2 = "Please confirm your password";
        } else if (values.password != values.password2){
            errors.password2 = "Passwords must match";
        }
        return errors;
    }

    function handleSubmit(values){
        console.log(values);
        fetch(window.location.origin + "/auth/signup", {
            method: "PUT",
            headers: {'Content-Type': 'application/json',},
            body: JSON.stringify({username: values.username, password: values.password})
        }).then(result => {
            if (result.status === 200) {
              setAuthTokens(result.data);
              setLoggedIn(true);
            } else {
              setIsError(result.body);
            }
        }).catch(e => {
            console.log(e);
        });
    }
    return (
        <Container className="page">
            <Form noValidate onSubmit={formik.handleSubmit}>
                <h2>Sign Up</h2>
                <Form.Group controlId="username">
                    <Form.Label>Username</Form.Label>
                    <Form.Control required type="text" placeholder="Enter legit anything"
                    value={formik.values.username}
                    onChange={formik.handleChange}
                    onBlur={formik.handleBlur}
                    isInvalid={formik.touched.username && formik.errors.username }/>
                    <Form.Control.Feedback type="invalid" >
                        Please choose a username
                    </Form.Control.Feedback>
                </Form.Group>
                <Form.Group controlId="password">
                    <Form.Label>Password</Form.Label>
                    <Form.Control required type="text" placeholder="Password"
                    value={formik.values.password}
                    onBlur={formik.handleBlur}
                    onChange={formik.handleChange}
                    isInvalid={formik.touched.password && formik.errors.password } />
                    <Form.Control.Feedback type="invalid">
                        Please choose a password
                    </Form.Control.Feedback>
                    <Form.Text className="text-muted">
                        We hash these passwords, but I'm not a security expert<br/>Please don't use your common password
                    </Form.Text>
                </Form.Group>
                <Form.Group controlId="password2">
                    <Form.Label>Re-enter Password</Form.Label>
                    <Form.Control required type="text" placeholder="You know the drill"
                    value={formik.values.password2}
                    onBlur={formik.handleBlur}
                    onChange={formik.handleChange}
                    isInvalid={formik.touched.password2 && formik.errors.password2 } />
                    <Form.Control.Feedback type="invalid">
                        {formik.errors.password2}
                    </Form.Control.Feedback>
                </Form.Group>
                <Form.Group controlId="email">
                    <Form.Label>Email</Form.Label>
                    <Form.Control type="text" placeholder="Psych bitch you thought we had email systems lmao" readOnly />
                </Form.Group>
                <Button variant="dark" type="submit">Let's Do It</Button>
            </Form>
        </Container>
    )
  }
  
  export default SignUpForm;