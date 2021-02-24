import React, {useContext, useState, useEffect} from "react";
import { AuthContext } from "./Context/Auth";
import { Container, Button, Alert} from "react-bootstrap";
import { Redirect } from "react-router";

import { Editor } from '@tinymce/tinymce-react';
import tinymce from 'tinymce/tinymce';
import 'tinymce/icons/default';
import 'tinymce/themes/silver';

import 'tinymce/plugins/paste';
import 'tinymce/plugins/link';
import 'tinymce/plugins/anchor';
import 'tinymce/plugins/code';
import 'tinymce/plugins/save';

//This page is put behind a private route, so it should only be accessed if the user authtokens are filled
function WritingPage(props) {
  let auth = useContext(AuthContext);
  let [isPublic, setIsPublic] = useState(false);
  let [showSaved, setShowSaved] = useState(false); //controls the "saved" alert
  let [showPrivate, setShowPrivate] = useState(false); //controls the alert that pops when the story is set to private
  const [shouldRedirect, setShouldRedirect] = useState(0);
  let id = window.location.pathname.substring(window.location.pathname.lastIndexOf("/") + 1);
  function load(){
    let path1 = window.location.pathname.substring(0,window.location.pathname.lastIndexOf("/")) //There is most likely a better way to do this
    let path = window.location.origin + "/" + path1.substring(0,path1.lastIndexOf("/")) + props.writerType /*story or bio*/;
    if(props.writerType=="story"){
    
      fetch(path + "/" + id + "/?t=t&id=" + id,{
        headers: {Authorization: auth.authTokens.head},})
      .then(response => response.text())
      .then(data => {if(data)tinyMCE.activeEditor.setContent(data)});
    }else{
    
      fetch("/bio/" + id,{
        headers: {Authorization: auth.authTokens.head},})
      .then(response => response.text())
      .then(data => {if(data)tinyMCE.activeEditor.setContent(data)});
    }
  }

  useEffect(() => {
    let path1 = window.location.pathname.substring(0,window.location.pathname.lastIndexOf("/")) //There is most likely a better way to do this
    let path = window.location.origin + "/" + path1.substring(0,path1.lastIndexOf("/")) + props.writerType /*story or bio*/;
    if(props.writerType=="story"){
      fetch(path + "/" + id + "/?t=m&id=" + id,{
        headers: {Authorization: auth.authTokens.head},})
      .then(response => response.json())
      .then(data => {
        data.permission == 1?setIsPublic(true):setIsPublic(false);
      });
    }
    else{
      auth.authTokens.bio== 2?setIsPublic(true):setIsPublic(false);
    }
  }, []); //this pattern of useEffect is basically componentDidMount
  
  const handleSubmit = () => { //When the save button is pressed
    if(!tinyMCE.activeEditor.isDirty()){//Only send data if we need to
      fetch(window.location.pathname, {method: 'PUT',
      headers: { Authorization: auth.authTokens.head,
      'Content-Type': 'text/html',},
      body: tinyMCE.activeEditor.getContent(),
      }).then(response =>{
        if (response.status == 201){
          setShowSaved(true);
          setTimeout(() => setShowSaved(false), 3500);
        }
        else{console.log('could not save');}
      });
    }
  };

  const publish = () => { //when the publish button is pressed
    fetch(window.location.pathname, {method: 'PUT',
      headers: { Authorization: auth.authTokens.head,
      'Content-Type': 'text/html',},
      body: tinyMCE.activeEditor.getContent(),
    }).then(fetch(window.location.pathname + "/publish", {method: 'POST',
    headers: { Authorization: auth.authTokens.head}})
    ).then(setShouldRedirect(id)); //redirect to the story page for this story
  }

  function testRedirect(){
    if (shouldRedirect != 0){
        return(
            <Redirect to= {"/story/" + shouldRedirect}/>
        );
    }
  }
  const makePrivate = () => {
    fetch(window.location.pathname + "/makeprivate", {method: 'POST',
      headers: { Authorization: auth.authTokens.head}
    }).then(response => {
      if (response.status == 200){
        setIsPublic(false);
        setShowPrivate(true);
        setTimeout(() => setShowPrivate(false), 4000);
      }
    });
  }
  
  function savedAlert() {
  
    if (showSaved) {
      return (
        <div>
          <br/>
          <Alert variant="success" onClose={() => setShowSaved(false)} dismissible>
            <Alert.Heading>Work has been saved!</Alert.Heading>
          </Alert>
        </div>
        
      );
    }
  }
  function privateAlert() {
  
    if (showPrivate) {
      return (
        <div>
          <br/>
          <Alert variant="success" onClose={() => setShowPrivate(false)} dismissible>
            <Alert.Heading>Your story is now visible only to you</Alert.Heading>
          </Alert>
        </div>
        
      );
    }
  }

  return(
    <Container className="page">
      {testRedirect()}
      <Container className="storyEditor" >
      {savedAlert()}
      {privateAlert()}
          <Editor 
            initialValue={"<p>Write your " + props.writerType + " here!</p>"}
            init={{
              plugins: [
                'link, paste, save, code',
              ],
              save_onsavecallback: handleSubmit,
              toolbar: 'save | undo redo | styleselect | code | bold italic | alignleft aligncenter alignright alignjustify | outdent indent',
              skin_url: '/skins',
              height: window.innerHeight - 200,
              skin: 'snow',
              theme: 'silver',
              branding: false,
              save_enablewhendirty: false,
              setup: load(),
              }}
          />  
      </Container> {/*storyEditor*/}
      {isPublic?
        <Button variant="dark" onClick={makePrivate} className="paddedButton">Make Private</Button>
        :<Button variant="dark" onClick={publish} className="paddedButton">Publish</Button>
      }
    </Container>
  );
}



export default WritingPage;