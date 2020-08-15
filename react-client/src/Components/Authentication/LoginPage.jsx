import React, { useState } from "react";
import { Link, Redirect } from "react-router-dom";
import { useAuth } from "../Context/Auth";
import { LinkContainer } from 'react-router-bootstrap';
import Nav from 'react-bootstrap/Nav';

function LoginPage() {
  const [isLoggedIn, setLoggedIn] = useState(false);
  const [isError, setIsError] = useState(false);
  const [userName, setUserName] = useState("");
  const [password, setPassword] = useState("");
  const { setAuthTokens } = useAuth();

  function postLogin() {
    fetch(indow.location.origin + "/auth/login", {method: "POST"})
    .then(result => {
      if (result.status === 200) {
        setAuthTokens(result.data);
        setLoggedIn(true);
      } else {
        setIsError(true);
      }
    }).catch(e => {
      setIsError(true);
      console.log(e)
    });
  }

  if (isLoggedIn) {
    return <Redirect to="/" />;
  }
  return(
    <div>
      <p>bro you should, like, log in</p>
      <LinkContainer to="/auth/signup"><Nav.Link className="headerNavButton">
        Don't have an account?  
      </Nav.Link></LinkContainer>
    </div>
  );
}

export default LoginPage;