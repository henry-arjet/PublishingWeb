import React from "react";
import { Route, Redirect } from "react-router-dom";
import { useAuth } from "./Context/Auth";

function PrivateRoute({ component: Component, ...rest }) {
  const {authTokens} = useAuth();
  //console.log(authTokens);
  return (
    <Route {...rest}
      render={props => false ?
        ( <Component {...props} />) 
        : ( <Redirect to="/auth/login" />)
      }
    />
  );
  /*return (
    <Route {...rest}
      render={props => authTokens.userName != "" ?
        ( <Component {...props} />) 
        : ( <Redirect to="/auth/login" />)
      }
    />
  );*/
}

export default PrivateRoute;