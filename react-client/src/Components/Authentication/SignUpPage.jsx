import React from 'react';
import Container from 'react-bootstrap/Container'
import SignUpForm from './SignUpForm'

function SignUpPage(){

    return(
        <Container className="page">
            <SignUpForm />
        </Container>
    );

}

export default SignUpPage;