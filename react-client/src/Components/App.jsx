import React, { Component } from 'react';
import { BrowserRouter as Router, Route, Link} from 'react-router-dom';
import HomePage from './HomePage';
import NavBar from './HeaderComponent/NavBar';
import Footer from './FooterComponent/Footer';
import MostViewedPage from './MostViewedPage';
import TopRatedPage from './TopRatedPage';
import StoryPage from './StoryComponent/StoryPage';
import DevPage from './DevTools/DevPage';
import AuthContext from "./Context/Auth";




class App extends Component {
  render() {
    return (
      <AuthContext.Provider value={false}>
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
              <Route name="devtools" path="/devtools">
                <DevPage />
              </Route>
            </switch>
            <Footer />
          </div>
        </Router>
      </AuthContext.Provider>
    )
  }
}
export default App;