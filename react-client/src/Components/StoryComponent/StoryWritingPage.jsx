//simple wrapper to give story-writer-unique props to the writing page

import React from "react";
import WritingPage from "../WritingPage"

function StoryWritingPage(){
    return (
        <WritingPage writerType="story" />
    );
}

export default StoryWritingPage;
