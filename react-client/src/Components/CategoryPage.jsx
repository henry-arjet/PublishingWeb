import React, {useState, useEffect } from 'react';
import CardGrid from './Cards/CardGrid'

function CategoryPage() 
{
    let [gotResults, setGotResults] = useState(false);
    let [results, setResults] = useState();
    const [blockFetch, setBlockFetch] = useState(false); //we only want to make one fetch request
  
  if(!blockFetch) {
    let numberR = parseInt(window.location.pathname.substring(window.location.pathname.lastIndexOf("/") + 1)); //either mask or page number
    let remainder = window.location.pathname.substring(0, window.location.pathname.lastIndexOf("/"));
    let numberL = parseInt(remainder.substring(remainder.lastIndexOf("/") + 1)); //either mask or category name, the latter is useless
    let page = 1;
    let mask = 0;
    if (numberL){//not NaN, so L must be mask and R must be page number
      mask = numberL;
      page = numberR;
    } else{
      mask = numberR;
      //page is 1 by default
    }
    /*
    console.log("numberR: " + numberR);
    console.log("remainder: " + remainder);
    console.log("numberL: " + numberL);
    console.log("page: " + page);
    console.log("mask: " + mask);
    */
   let pageLink = window.location.protocol + "//" + window.location.host + '/results/?o=mv&p=' + page + "&c=" + mask;
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


    if(!blockFetch) {
        setBlockFetch(true);
        let mask = window.location.pathname.substring(window.location.pathname.lastIndexOf("/") + 1);
        fetch(window.location.protocol + "//" + window.location.host + '/results/?o=mv&c=' + mask)
        .then(response => response.json())
        .then(data => {setResults(data), setGotResults(true)});
    }
    
    if(gotResults==true){
        return (
          <CardGrid results = {results} setBlockFetch={setBlockFetch} />    
        );
      } 
    else return(
        <div>
          <br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
          <br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
        </div>
      );    
}

export default CategoryPage;