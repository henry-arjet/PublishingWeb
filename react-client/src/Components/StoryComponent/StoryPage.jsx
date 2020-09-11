import React, {useContext, useState } from 'react';
import { Container, Button } from 'react-bootstrap';
import { AuthContext } from "../Context/Auth";
import { useEffect } from 'react';
import StoryMeta from "./StoryMeta";



function StoryPage() {
  const [html, setHtml] = useState(null);
  const [gotResults, setGotResults] = useState(false);
  let auth = useContext(AuthContext);

  useEffect(() => {
    let completed = 0; //counts if both 
    fetch(window.location.pathname + "?t=t&id=" + window.location.pathname.substring(window.location.pathname.lastIndexOf("/") + 1),
      {headers: {Authorization: auth.authTokens.head,},})
      .then(response => response.text())
      .then(data => {
        setHtml(data);  
        setGotResults(true);
      });
  }, []);//only calls on mount
  if (gotResults){
    return (
      <Container className="page" >
        <StoryMeta/>
        <br/><br/>
        <div dangerouslySetInnerHTML={{__html: html}}/> {/*This html runs through a sanitizer before being stored by the server*/}
      </Container>
    )
  }
  return (
    <div>
    <br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
    <br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
    <br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
    </div>
  )
}
  
  export default StoryPage;