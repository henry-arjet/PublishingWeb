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
  const [userRetrieved, setUserRetrieved] = useState({});
  const [name, setName] = useState(0);
  const [bioHTML, setBioHTML] = useState("");
  const [blockUserFetch, setBlockUserFetch] = useState(false); //used to make sure only one request for user info and bio is sent
  const pageID = location.pathname.substring(location.pathname.lastIndexOf("/") + 1);
  const isSelf = pageID == auth.authTokens.id?true:false;

  if (pageID != userRetrieved && !blockUserFetch){//if we haven't loaded the results for this user.
                               //React router means user can change without component remounting
    setBlockUserFetch(true); // kinda semaphore
    let page =parseInt(window.location.pathname.substring(window.location.pathname.lastIndexOf("/") + 1)); //gets page number from url
    if (!page){page = 1;} //if no page number, its 1

    let path =location.pathname + '/stories/?p=' + page;
    isSelf?path += '&a=self':path+= '&a=public';
    fetch(path, { headers: { Authorization: auth.authTokens.head,},})
    .then(response => response.json())
    .then(data => {
      setResults(data.results);
      setName(data.name);
      setUserRetrieved(pageID); 
      setBlockUserFetch(false);
    });
    fetch("/bio/" + pageID, {headers: {Authorization: auth.authTokens.head,},}).then(response =>{
        if (response.status==200) response.text().then(data => setBioHTML(data));
        }
      );
    }
  function logout(){
    localStorage.removeItem("tokens"); auth.authTokens = null;
    auth.setAuthTokens({ id: 0, username: "", password: "", privilege: 0, head: ""});
    return(<Redirect to="/" />);
  }
  
  const bio = () => {
    if (!bioHTML) return;
    else return (<div dangerouslySetInnerHTML={{__html: bioHTML}}/>);
  }

  const selfElements = () => {
    if (isSelf){
      return (
        <div>
          <h3>Welcome, {auth.authTokens.username}</h3>
          <LinkContainer to="/new/meta"><Button variant="dark" className="paddedButton"> Upload New Story</Button></LinkContainer>
          {<LinkContainer to={"/biowriter/" + pageID}><Button variant="dark" className="paddedButton"> Edit Bio</Button></LinkContainer>
            }
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
  
  if(userRetrieved){ //return the user's stories if they have been loaded
    return(
      <Container className="page">
        {selfElements()}
        {bio()}
        <CardGrid results = {results} setBlockFetch = {setBlockUserFetch} supressSelect={true}/>
      </Container>
    );
  }else return(
    <div>
      Fetching profile...
    </div>
  );
}


export default UserPage;