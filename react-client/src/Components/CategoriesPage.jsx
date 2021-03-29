import React, { Component, useState, useEffect } from 'react';
import CardGrid from './Cards/CardGrid'

function CategoriesPage() {
    let [gotResults, setGotResults] = useState(false);
    let [results, setResults] = useState();
    const [blockFetch, setBlockFetch] = useState(false); //we only want to make one fetch request

    if(!blockFetch) {
        setBlockFetch(true);
        fetch(window.location.protocol + "//" + window.location.host + '/cats.json')
        .then(response => response.json())
        .then(data => {setResults(data), setGotResults(true)});
    }
    if(gotResults)return(<CardGrid categories={results}  setBlockFetch={setBlockFetch} supressSelect={true}/>);
    else return(<div>
        <br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
        <br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
      </div>)
}

export default CategoriesPage;