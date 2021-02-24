//simple wrapper to give story-writer-unique props to the writing page

import React from "react";
import WritingPage from "../WritingPage"

function BioWritingPage(){
    return (
        <WritingPage writerType="bio" />
    );
}

export default BioWritingPage;