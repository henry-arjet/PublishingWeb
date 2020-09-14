import React, {useContext, useState, useEffect} from "react";
import { AuthContext } from "../Context/Auth";
import { Container, Button} from "react-bootstrap";
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
function WritingPage() {
  let auth = useContext(AuthContext);
  let [meta, setMeta] = useState(null);
  let [isPublic, setIsPublic] = useState(false);
  const [shouldRedirect, setShouldRedirect] = useState(0);

  function load(){
    console.log("loading");
    let id = window.location.pathname.substring(window.location.pathname.lastIndexOf("/") + 1);
    let path1 = window.location.pathname.substring(0,window.location.pathname.lastIndexOf("/")) //There is most likely a better way to do this
    let path = window.location.origin + "/" + path1.substring(0,path1.lastIndexOf("/")) + "story";
    fetch(path + "/" + id + "/?t=t&id=" + id,{
      headers: {Authorization: auth.authTokens.head},})
    .then(response => response.text())
    .then(data => {if(data)tinyMCE.activeEditor.setContent(data)});
  }

  useEffect(() => {
    let id = window.location.pathname.substring(window.location.pathname.lastIndexOf("/") + 1);
    let path1 = window.location.pathname.substring(0,window.location.pathname.lastIndexOf("/")) //There is most likely a better way to do this
    let path = window.location.origin + "/" + path1.substring(0,path1.lastIndexOf("/")) + "story";
    fetch(path + "/" + id + "/?t=m&id=" + id,{
      headers: {Authorization: auth.authTokens.head},})
    .then(response => response.json())
    .then(data => {
      setMeta(data);
      data.permission == 1?setIsPublic(true):setIsPublic(false);
    });
  }, []); //this pattern of useEffect is basically componentDidMount
  
  const handleSubmit = () => { //When the save button is pressed
    if(!tinyMCE.activeEditor.isDirty()){//Only send data if we need to
      fetch(window.location.pathname, {method: 'PUT',
      headers: { Authorization: auth.authTokens.head,
      'Content-Type': 'text/html',},
      body: tinyMCE.activeEditor.getContent(),
    })//Really need to add feedback to show that they saved
    }
  };

  function testRedirect(){
    if (shouldRedirect != 0){
        return(
            <Redirect to= {"/story/" + shouldRedirect}/>
        );
    }
  }

  const publish = () => {
    handleSubmit();
    fetch(window.location.pathname + "/publish", {method: 'POST',
      headers: { Authorization: auth.authTokens.head}
    }).then(setShouldRedirect(meta.id));
    
  }
  const makePrivate = () => {
    fetch(window.location.pathname + "/makeprivate", {method: 'POST',
      headers: { Authorization: auth.authTokens.head}
    });
  }
  
  return(
    <Container className="page">
      {testRedirect()}
      <Container className="storyEditor" >
          <Editor 
            initialValue="<p>Write your story here!</p>"
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