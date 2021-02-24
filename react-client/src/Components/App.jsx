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
import PrivateRoute from "./PrivateRoute";
import LoginPage from "./Authentication/LoginPage";
import SignUpPage from './Authentication/SignUpPage';
import UserPage from './User/UserPage';
import StoryWritingPage from './StoryComponent/StoryWritingPage';
import BioWritingPage from './User/BioWritingPage';
import StoryMetaCreator from './StoryComponent/StoryMetaCreator';
import NewestPage from './NewestPage';



function App(props){
    const existingTokens = JSON.parse(localStorage.getItem("tokens"));
    const backup = { id: 0, username: "", password: "", privilege: 0, head: ""};
    const [authTokens, setAuthTokens] = useState(existingTokens?existingTokens:backup); //if none found, use default
    const setTokens = (data) => { //I don't think I still use this?
      localStorage.setItem("tokens", JSON.stringify(data));
      setAuthTokens(data);
    }
    return (
      <AuthContext.Provider value={{authTokens, setAuthTokens}}>
        <Router>
          <div>
            <NavBar />
            <switch>
              <Route name="home" exact path="/"component={HomePage} />
              <Route name="toprated" path="/toprated" component={TopRatedPage} />
              <Route name="mostviewed" path="/mostviewed" component={MostViewedPage}/>
              <Route name="story" exact path="/story/*" component={StoryPage} />
              <Route name="new story meta" path="/new/meta" component={StoryMetaCreator}/>
              <PrivateRoute name="devtools" path="/devtools" component={DevPage} />
              <Route name="userpage" path="/users/" component={UserPage} />
              <Route name="login" path="/auth/login" component={LoginPage}/>
              <Route name="signup" path="/auth/signup" component={SignUpPage}/>
              <PrivateRoute name="writer" path='/writer' component={StoryWritingPage} />
              <PrivateRoute name="biowriter" path="/biowriter" component={BioWritingPage} />
              <Route name="newest" path="/newest" component={NewestPage}/>
            </switch>
            <Footer />
          </div>
        </Router>
      </AuthContext.Provider>
    )
  }

export default App;

