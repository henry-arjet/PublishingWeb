import React, {useContext, useState, useEffect } from 'react';
import { Button } from 'react-bootstrap';
import { AuthContext } from "../Context/Auth";
import { LinkContainer } from 'react-router-bootstrap';
import Rate from "./Rate"



function StoryMeta() {
  const [meta, setMeta] = useState(null);
  const [gotResults, setGotResults] = useState(false);

  let auth = useContext(AuthContext);

  useEffect(() => {
    fetch(window.location.pathname + "?t=m&id=" + window.location.pathname.substring(window.location.pathname.lastIndexOf("/") + 1),
      {headers: {Authorization: auth.authTokens.head,},})
      .then(response => response.json())
      .then(data => {
        setMeta(data);  
        setGotResults(true);
    });
  }, []);//only calls on mount

  function EditStoryButton(){ //If the user is the author or an admin, let them edit the story
    if (meta.authorID == auth.authTokens.id || auth.authTokens.privilege > 3){ //3 is arbitrary. 
      return(
        <div>
          <LinkContainer to={"/writer/" + meta.id} ><Button variant="dark"> Edit Story </Button></LinkContainer>
        </div>
      );
    }
    else return;
  }
  function Views(){
      return <div className="packedP"><b>{meta.views} Views</b></div>
  }
  function Rating(){
      if (meta.rating == 0){ //unrated
        return(<div className="packedP"><b>Unrated</b></div>);
      }
      else{
          let r = (meta.rating / 10000).toFixed(1).toString();
          return(<div className="packedP"><b>{r + "/5.0"}</b></div>);
      }
  }
  function Stars(){
      if(auth.authTokens.id != 0){ //if the user is logged in
          return(
            <Rate meta={meta} />
          )
      }
      else return null;
  }

  if (gotResults){ //happens once the html and the metadata for the story have both come back from the server
    return (
      <div>
        {Views()}
        {Rating()}
        {Stars()}
        {EditStoryButton()}
      </div>
    )
  }
  else return null;
}
  
  export default StoryMeta;