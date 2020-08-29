import React, {useContext, useState, useEffect} from "react";
import { AuthContext } from "../Context/Auth";
import CardGrid from "../Cards/CardGrid";

//This page is put behind a private route, so it should only be accessed if the user authtokens are filled
function UserPage() {
  let auth = useContext(AuthContext);
  const [results, setResults] = useState({});
  const [gotResults, setGotResults] = useState(false);

  useEffect(() => {
    fetch(window.location.protocol + "//" + window.location.host + location.pathname + '/stories?p=1', {
    headers: { Authorization: "Basic " + btoa(auth.authTokens.id + ":" + auth.authTokens.password),},
    }).then(response => response.json()).then(data => setResults(data), setGotResults(true));
  }, []);

  
  
    //  .then(response => response.json()).then(data => setResults(data), setGotResults(true));
  if(gotResults){
    return(
      <div>
        <p>Welcome, {auth.authTokens.username}</p>
        <CardGrid results={results} />
      </div>
    );
  }//else
  return(
    <div>
      Please log in. In fact, how are you here without being logged in? Mad suss
    </div>
  );
}

export default UserPage;