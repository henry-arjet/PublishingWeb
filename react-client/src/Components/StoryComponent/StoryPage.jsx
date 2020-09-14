import React, {useContext, useState } from 'react';
import { Container, Button, Col } from 'react-bootstrap';
import { AuthContext } from "../Context/Auth";
import { useEffect } from 'react';
import StoryMeta from "./StoryMeta";



function StoryPage() {
  const [html, setHtml] = useState(null);
  const [gotResults, setGotResults] = useState(false);
  let auth = useContext(AuthContext);

  useEffect(() => {
    let completed = 0; //counts if both 
    fetch(window.location.pathname + "/?t=t&id=" + window.location.pathname.substring(window.location.pathname.lastIndexOf("/") + 1),
      {headers: {Authorization: auth.authTokens.head,},})
      .then(response => response.text())
      .then(data => {
        setHtml(data);  
        setGotResults(true);
    });
    fetch(window.location.pathname + "/?t=v", {method:"PUT"});//add a view
    
  }, []);//only calls on mount
  if (gotResults){
    return (
      <Container className="page" >
        <Container className="story">
          <Col m='8'>
            <StoryMeta/>
            <br/><br/>
            <div dangerouslySetInnerHTML={{__html: html}}/> {/*This html runs through a sanitizer before being stored by the server*/}
          </Col>
        </Container>
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