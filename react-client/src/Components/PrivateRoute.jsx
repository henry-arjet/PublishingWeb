import React, {useContext} from "react";
import { Route, Redirect } from "react-router-dom";
import { AuthContext } from "./Context/Auth";

function PrivateRoute({ component: Component, ...rest }) {
  let auth = useContext(AuthContext);
  return (
    <Route {...rest}
    render={props => {
      if (auth.authTokens) { return ( <Component {...props} />)}
      else return ( <Redirect to="/auth/login" />);
    }
    }
    />
  );
}

export default PrivateRoute;
