import React, { useState } from 'react';
import { BrowserRouter as Router, Route } from 'react-router-dom';
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
import UserPage from './User/UserPage';
import WritingPage from './StoryComponent/WritingPage';
import StoryMetaCreator from './StoryComponent/StoryMetaCreator';

function App(props){
    const existingTokens = JSON.parse(localStorage.getItem("tokens"));
    const backup = {authTokens: { id: 0, username: "", password: "", privilege: 0, head: ""}};
    const [authTokens, setAuthTokens] = useState(existingTokens?existingTokens:backup); //if none found, use default

    const setTokens = (data) => {
      localStorage.setItem("tokens", JSON.stringify(data));
      setAuthTokens(data);
    }
    return (
      <AuthContext.Provider value={{authTokens, setAuthTokens}}>
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
              <Route name="story" exact path="/story/*">
                <StoryPage />
              </Route>
              <Route name="new story meta" path="/new/meta" component={StoryMetaCreator}/>
              <PrivateRoute name="devtools" path="/devtools" component={DevPage} />
              <PrivateRoute name="userpage" path="/users/profile" component={UserPage} />
              <Route name="login" path="/auth/login" component={LoginPage}/>
              <Route name="signup" path="/auth/signup" component={SignUpPage}/>
              <PrivateRoute name="writer" path="/writer" component={WritingPage}/>
            </switch>
            <Footer />
          </div>
        </Router>
      </AuthContext.Provider>
    )
  }

export default App;

