import React, { useState, useEffect } from 'react';
import CardGrid from './Cards/CardGrid'

function NewestPage() {
    let [results, setResults] = useState([]);
    let [gotResults, setGotResults] = useState(false);

    useEffect(() => {
        fetch(window.location.protocol + "//" + window.location.host + '/results/?o=new')
            .then(response => response.json()).then(data => { setResults(data); setGotResults(true) });
    }, []);

    if (gotResults) {
        return (
            <CardGrid results={results} />
        );
    } else return (
        <div>
            <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
            <br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
        </div>
    );

}

export default NewestPage;