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
  const [isSelf, setIsSelf] = useState(false); //is this the user's own page
  const pageID = location.pathname.substring(location.pathname.lastIndexOf("/") + 1);

  useEffect(() => {
    
    fetch(window.location.protocol + "//" + window.location.host + location.pathname + '/stories?p=1', {
    headers: { Authorization: auth.authTokens.head,},
    }).then(response => response.json())
    .then(data => {
      setResults(data);
      setGotResults(true); 
      });
    gotResults.id == auth.authTokens.id?setIsSelf(true):setIsSelf(false);
  }, []); //this pattern of useEffect is basically componentDidMount

  function logout(){
    localStorage.removeItem("tokens"); auth.authTokens = null;
    auth.setAuthTokens(null);
    setLoggedIn(false);
  }
  
  const selfElements = () => {
    if (isSelf){
      return (
        <div>
          <p>Welcome, {auth.authTokens.username}</p>
          <LinkContainer to="/new/meta"><Button variant="dark" className="paddedButton"> Upload New Story</Button></LinkContainer>
          <Link to="/" onClick={logout} >Log Out</Link>
        </div>
      );
    }
  }
  
  if(isLoggedIn){
    if(gotResults){
      return(
        <Container className="page">
          {selfElements}
          
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