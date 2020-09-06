import React, {useContext} from "react";
import { AuthContext } from "../Context/Auth";
import { Container} from "react-bootstrap";

import { Editor } from '@tinymce/tinymce-react';
import tinymce from 'tinymce/tinymce';
import 'tinymce/icons/default';
import 'tinymce/themes/silver';

import 'tinymce/plugins/paste';
import 'tinymce/plugins/link';
import 'tinymce/plugins/anchor';
import 'tinymce/plugins/save';



//This page is put behind a private route, so it should only be accessed if the user authtokens are filled
function WritingPage() {
  let auth = useContext(AuthContext);

  function load(){
    let id = window.location.pathname.substring(window.location.pathname.lastIndexOf("/") + 1);
    let path1 = window.location.pathname.substring(0,window.location.pathname.lastIndexOf("/")) //There is most likely a better way to do this
    let path = window.location.origin + "/" + path1.substring(0,path1.lastIndexOf("/")) + "story";
    fetch(path + "/" + id + "?t=t&id=" + id,{
      headers: {Authorization: "Basic " + btoa(auth.authTokens.id + ":" + auth.authTokens.password),},})
    .then(response => response.text())
    .then(data => {if(data)tinyMCE.activeEditor.setContent(data)});
  }

  const handleSubmit = () => { //When the save button is pressed
    if(!tinyMCE.activeEditor.isDirty()){//Only send data if we need to
      fetch(window.location.pathname, {method: 'PUT',
      headers: { Authorization: "Basic " + btoa(auth.authTokens.id + ":" + auth.authTokens.password),
      'Content-Type': 'text/html',},
      body: tinyMCE.activeEditor.getContent(),
    })//Really need to add feedback to show that they saved
    }
  };
  
  return(
    <div>
      <Container className="storyEditor" >
          <Editor 
            initialValue="<p>This is the initial content of the editor</p>"
            init={{
              plugins: [
                'link, paste, anchor, save',
              ],
              save_onsavecallback: handleSubmit,
              toolbar: 'save | undo redo | styleselect | bold italic | alignleft'
              + 'aligncenter alignright alignjustify | outdent indent | anchor',
              skin_url: '/skins',
              height: window.innerHeight - 200,
              skin: 'snow',
              theme: 'silver',
              branding: false,
              save_enablewhendirty: false,
              setup: load(),
              }}
          />  
      </Container>
    </div>
  );
}

export default WritingPage;