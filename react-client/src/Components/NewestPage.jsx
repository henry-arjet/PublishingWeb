import React, { useState, useEffect } from 'react';
import CardGrid from './Cards/CardGrid'

function NewestPage() {
  const [results, setResults] = useState([]);
  const [gotResults, setGotResults] = useState(false);
  const [blockFetch, setBlockFetch] = useState(false); //we only want to make one fetch request
  let page =parseInt(window.location.pathname.substring(window.location.pathname.lastIndexOf("/") + 1)); //gets page number from url
  if (!page){page = 1;} //if no page number, its 1
  let pageLink = window.location.protocol + "//" + window.location.host + '/results/?o=new&p=' + page;
  if(!blockFetch) {
    setBlockFetch(true);
    setGotResults(false);
    fetch(pageLink).then(response => response.json()).then(data => {setResults(data); setGotResults(true)});
  };

  if(gotResults){
    return (
        <CardGrid results = {results} setBlockFetch = {setBlockFetch}/>
    )
  } else return(
    <div>
      <br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
      <br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
    </div>
  )
}

export default NewestPage;