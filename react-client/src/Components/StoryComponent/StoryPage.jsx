import React, {useContext, useState } from 'react';
import { Container, Button } from 'react-bootstrap';
import { AuthContext } from "../Context/Auth";
import { useEffect } from 'react';
import { LinkContainer } from 'react-router-bootstrap';



function StoryPage() {
  const [html, setHtml] = useState(null);
  const [meta, setMeta] = useState(null);
  const [gotResults, setGotResults] = useState(false);
  let auth = useContext(AuthContext);

  useEffect(() => {
    let completed = 0; //counts if both 
    fetch(window.location.pathname + "?t=t&id=" + window.location.pathname.substring(window.location.pathname.lastIndexOf("/") + 1),
      {headers: {Authorization: auth.authTokens.head,},})
      .then(response => response.text())
      .then(data => {setHtml(data);  
        completed+=1; //Race condition. Should be fine, especially since js is single threaded 
        if(completed==2){setGotResults(true)}//Id love to do this another way, but I tried and it doesn't work.
      });
    fetch(window.location.pathname + "?t=m&id=" + window.location.pathname.substring(window.location.pathname.lastIndexOf("/") + 1),
      {headers: {Authorization: auth.authTokens.head,},})
      .then(response => response.json())
      .then(data => {setMeta(data);  
        completed+=1; 
        if(completed==2){setGotResults(true)}
    });
  }, []);//only calls on mount

  function EditStoryButton(){ //If the user is the author or an admin, let them edit the story
    if (meta.authorID == auth.authTokens.id || auth.authTokens.privilege > 3){ //3 is arbitrary. 
      return(
        <div>
          <br/>
          <LinkContainer to={"/writer/" + meta.id} ><Button variant="dark"> Edit Story </Button></LinkContainer>
        </div>
      );
    }
    else return;
  }

  if (gotResults){ //happens once the html and the metadata for the story have both come back from the server
    return (
      <Container className="page" >
        {EditStoryButton()}
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