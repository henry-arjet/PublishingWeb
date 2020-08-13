import React from "react";
import { Route, Redirect } from "react-router-dom";
import { useAuth } from "./Context/Auth";

function PrivateRoute({ component: Component, ...rest }) {
  const isAuthenticated = useAuth();

  return (
    <Route {...rest}>
        {props => isAuthenticated ?
        ( <Component {...props} />) 
        : ( <Redirect to="/" />)
      }
    </Route>
  );
}

export default PrivateRoute;