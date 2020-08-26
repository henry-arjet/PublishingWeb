import React from "react";
import { LinkContainer } from 'react-router-bootstrap';
import Nav from 'react-bootstrap/Nav';
import LoginForm from './LoginForm'
import { Container } from "react-bootstrap";

function LoginPage() {
  return(
    <div>
      <p>bro you should, like, log in</p>
      <Container classname="page">
        <LoginForm/>
        <LinkContainer to="/auth/signup"><Nav.Link className="headerNavButton">
          Don't have an account?  
        </Nav.Link></LinkContainer>
      </Container>
    </div>
  );
}

export default LoginPage;