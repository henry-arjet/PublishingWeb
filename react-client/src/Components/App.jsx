import React, { Component, useState } from 'react';
import { BrowserRouter as Router, Route, Link} from 'react-router-dom';
import HomePage from './HomePage';
import NavBar from './HeaderComponent/NavBar';
import Footer from './FooterComponent/Footer';
import MostViewedPage from './MostViewedPage';
import TopRatedPage from './TopRatedPage';
import StoryPage from './StoryComponent/StoryPage';
import DevPage from './DevTools/DevPage';
import {AuthContext} from "./Context/Auth";
import PrivateRoute from "./PrivateRoute"
import LoginPage from "./Authentication/LoginPage"
import SignUpPage from './Authentication/SignUpPage';

function App(props){
    const existingTokens = JSON.parse(localStorage.getItem("tokens"));
    const [authTokens, setAuthTokens] = useState(existingTokens);
    
    const setTokens = (data) => {
      localStorage.setItem("tokens", JSON.stringify(data));
      setAuthTokens(data);
    }
    return (
      <AuthContext.Provider value={{authTokens, setAuthTokens: setTokens }}>
        <Router>
          <div>
            <NavBar />
            <switch>
              <Route name="home" exact path="/">
                <HomePage />
              </Route>
              <Route name="toprated" path="/toprated">
                <TopRatedPage />
              </Route>
              <Route name="mostviewed" path="/mostviewed">
                <MostViewedPage />
              </Route>
              <Route name="story" path="/story">
                <StoryPage />
              </Route>
              <PrivateRoute name="devtools" path="/devtools" component={DevPage} />
              <Route name="login" path="/auth/login" component={LoginPage}/>
              <Route name="signup" path="/auth/signup" component={SignUpPage}/>
            </switch>
            <Footer />
          </div>
        </Router>
      </AuthContext.Provider>
    )
  }

export default App;

/*              <Route name="devtools" path="/devtools">
                <DevPage />
              </Route>*/