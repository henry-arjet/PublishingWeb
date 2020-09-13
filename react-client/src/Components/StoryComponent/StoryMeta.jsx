import React, {useContext, useState, useEffect } from 'react';
import { Button } from 'react-bootstrap';
import { AuthContext } from "../Context/Auth";
import { Link } from 'react-router-dom'
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


  function timeString(upTime){
    console.log(upTime);
    let epoch = Date.now();
    let mils = epoch-upTime; //how many miliseconds ago was it uploaded
    if (mils > 31536000000){
        let ret = Math.floor(mils/31536000000);
        if (ret == 1){return ret + " year ago"}
        return ret + " years ago"
    }
    else if (mils > 2592000000 ){
        let ret = Math.floor(mils/2592000000);
        if (ret == 1){return ret + " month ago"}
        return ret + " months ago"
    }
    else if (mils > 604800000 ){
        let ret = Math.floor(mils/604800000);
        if (ret == 1){return ret + " week ago"}
        return ret + " weeks ago"
    }
    
    else if (mils > 86400000 ){
        let ret = Math.floor(mils/86400000);
        if (ret == 1){return ret + " day ago"}
        return ret + " days ago"
    }
    else if (mils > 3600000 ){
        let ret = Math.floor(mils/3600000);
        if (ret == 1){return ret + " hour ago"}
        return ret + " hours ago"
    }
    else if (mils > 60000 ){
        let ret = Math.floor(mils/60000);
        if (ret == 1){return ret + " minute ago"}
        return ret + " minutes ago"
    }
    else{
        let ret = Math.floor(mils/1000);
        if (ret == 1){return ret + " second ago"}
        return ret + " seconds ago"
    }

}

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
  function Timestamp(){
    let ts = timeString(meta.timestamp)
    return(<div className="packedP"><b>{ts}</b></div>);
  }
  function Stars(){
    if(auth.authTokens.id != 0){ //if the user is logged in
      return(
        <Rate meta={meta} />
      )
    }
    else return null;
  }
  function Author(){
    return( 
    <h3 className = "inline">
      By: <Link to={"/users/" + meta.authorID}>{meta.authorName}</Link>
    </h3>
    
    );
  }

  if (gotResults){ //happens once the html and the metadata for the story have both come back from the server
    return (
      <div>
        {<h2>{meta.title}</h2>}
        {Author()}
        {Views()}
        {Timestamp()}
        {Rating()}
        {Stars()}
        {EditStoryButton()}
      </div>
    )
  }
  else return null;
}
  
  export default StoryMeta;