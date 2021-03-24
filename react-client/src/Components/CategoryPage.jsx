import React, {useState, useEffect } from 'react';
import CardGrid from './Cards/CardGrid'

function CategoryPage() 
{
    let [gotResults, setGotResults] = useState(false);
    let [results, setResults] = useState();
    useEffect(() => {
        let mask = window.location.pathname.substring(window.location.pathname.lastIndexOf("/") + 1);
        fetch(window.location.protocol + "//" + window.location.host + '/results/?o=mv&c=' + mask)
        .then(response => response.json())
        .then(data => {setResults(data), setGotResults(true)});
    }, []);
    
    if(gotResults==true){
        return (
          <CardGrid results = {results} />    
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