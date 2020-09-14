import React, {useContext, useState, useEffect} from "react";
import {Link, Redirect} from "react-router-dom";
import { AuthContext } from "../Context/Auth";
import CardGrid from "../Cards/CardGrid";
import { Container, Button } from "react-bootstrap";
import { LinkContainer } from "react-router-bootstrap";

//This page is put behind a private route, so it should only be accessed if the user authtokens are filled
function UserPage() {
  let auth = useContext(AuthContext);
  const [results, setResults] = useState({});
  const [name, setName] = useState("");
  const [gotResults, setGotResults] = useState(false);
  //const [isSelf, setIsSelf] = useState(false); //is this the user's own page
  const pageID = location.pathname.substring(location.pathname.lastIndexOf("/") + 1);
  const isSelf = pageID == auth.authTokens.id?true:false;
  useEffect(() => {
    let path =location.pathname + '/stories/?p=1';
    isSelf?path += '&a=self':path+= '&a=public';
    fetch(path, { headers: { Authorization: auth.authTokens.head,},})
    .then(response => response.json())
    .then(data => {
      setResults(data.results);
      setName(data.name);
      setGotResults(true); 
      });
    //gotResults.id == auth.authTokens.id?setIsSelf(true):setIsSelf(false);
  }, []); //this pattern of useEffect is basically componentDidMount

  function logout(){
    localStorage.removeItem("tokens"); auth.authTokens = null;
    auth.setAuthTokens({ id: 0, username: "", password: "", privilege: 0, head: ""});
    return(<Redirect to="/" />);
  }
  
  const selfElements = () => {
    if (isSelf){
      return (
        <div>
          <h3>Welcome, {auth.authTokens.username}</h3>
          <LinkContainer to="/new/meta"><Button variant="dark" className="paddedButton"> Upload New Story</Button></LinkContainer>
          <Link to="/" onClick={logout} >Log Out</Link>
        </div>
      );
    }
    else return(
      <div>
        <h2>{name}'s Page</h2>
        <br></br>
      </div>
    );
  }
  
  if(gotResults){
    return(
      <Container className="page">
        {selfElements()}
        
        <CardGrid results={results} />
      </Container>
    );
  }else return(
    <div>
      Fetching profile...
    </div>
  );
}


export default UserPage;