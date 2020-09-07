import React, {useContext, useState, useEffect} from "react";
import {Link} from "react-router-dom";
import { AuthContext } from "../Context/Auth";
import CardGrid from "../Cards/CardGrid";
import { Container, Button } from "react-bootstrap";
import { LinkContainer } from "react-router-bootstrap";

//This page is put behind a private route, so it should only be accessed if the user authtokens are filled
function UserPage() {
  let auth = useContext(AuthContext);
  const [results, setResults] = useState({});
  const [gotResults, setGotResults] = useState(false);
  const [isLoggedIn, setLoggedIn] = useState(true);


  useEffect(() => {
    fetch(window.location.protocol + "//" + window.location.host + location.pathname + '/stories?p=1', {
    headers: { Authorization: auth.authTokens.head,},
    }).then(response => response.json()).then(data => setResults(data), setGotResults(true));
  }, []); //this pattern of useEffect is basically componentDidMount

  function logout(){
    localStorage.removeItem("tokens"); auth.authTokens = null;
    auth.setAuthTokens(null);
    setLoggedIn(false);
  }
  
    //  .then(response => response.json()).then(data => setResults(data), setGotResults(true));
  if(isLoggedIn){
    if(gotResults){
      return(
        <Container className="page">
          <p>Welcome, {auth.authTokens.username}</p>
          <LinkContainer to="/new/meta"><Button variant="dark" className="paddedButton"> Upload New Story</Button></LinkContainer>
          <Link to="/" onClick={logout} >Log Out</Link>
          <CardGrid results={results} />
        </Container>
      );
    }else return(
      <div>
        Fetching your profile...
      </div>
    );
  }
  else {
    return <Redirect to="/" />;
  }
}

export default UserPage;