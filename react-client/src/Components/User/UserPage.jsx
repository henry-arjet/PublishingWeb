import React, {useContext, useState, useEffect} from "react";
import { AuthContext } from "../Context/Auth";
//This page is put behind a private route, so it should only be accessed if the user authtokens are filled
function UserPage() {
  let auth = useContext(AuthContext);
  const [results, setResults] = useState({});
  const [gotResults, setGotResults] = useState(false);

  useEffect(() => {
    fetch(window.location.protocol + "//" + window.location.host + '/users/profile?t=profile&id=' + auth.authTokens.id, {
    headers: { Authorization: "Basic " + btoa(auth.authTokens.id + ":" + auth.authTokens.password),},
    });
  }, []);

  
  
    //  .then(response => response.json()).then(data => setResults(data), setGotResults(true));
  if(gotResults){
    return(
      <p>Welcome, {auth.authTokens.username}</p>
    );
  }//else
  return(
    <div>
      Please log in. In fact, how are you here without being logged in? Mad suss
    </div>
  );
}

export default UserPage;