import React, {useContext} from "react";
import { AuthContext } from "./Context/Auth";
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

  const handleSubmit = () => {
    console.log(tinyMCE.activeEditor.getContent());
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
              }}
          />  
      </Container>
    </div>
  );
}

export default WritingPage;