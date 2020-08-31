import React, { useContext } from 'react';
import Navbar from 'react-bootstrap/Navbar';
import Nav from 'react-bootstrap/Nav';
import { LinkContainer } from 'react-router-bootstrap';
import { AuthContext } from '../Context/Auth';


function NavBar() {
  let auth = useContext(AuthContext);
  function ProfileButton () {
    if(auth.authTokens != null){ //check to see if we're logged in
      return(
        <LinkContainer to="/users/profile"><Nav.Link className="headerNavButton">
          <strong>My Profile</strong>
        </Nav.Link></LinkContainer>
      );  
    }else {
      return(
        <LinkContainer to="/auth/login"><Nav.Link className="headerNavButton">
          <strong>Log In</strong>
        </Nav.Link></LinkContainer>
      );
    }
    
  }
  return (
    <header fluid>
      <Navbar bg="light"> 
        <LinkContainer to="/"><Navbar.Brand><strong>Publishing Website</strong></Navbar.Brand></LinkContainer>
        <Navbar.Collapse>
          <Nav className="headerNav">
          <LinkContainer to="/toprated"><Nav.Link className="headerNavButton">
            <strong>Top Rated</strong>
          </Nav.Link></LinkContainer>
          <LinkContainer to="/mostviewed"><Nav.Link className="headerNavButton">
            <strong>Most Viewed</strong>
          </Nav.Link></LinkContainer>
          <LinkContainer to="/devtools"><Nav.Link className="headerNavButton">
            <strong>Dev Tools</strong>
          </Nav.Link></LinkContainer>
          {ProfileButton()}
          </Nav>
        </Navbar.Collapse>
      </Navbar>
    </header>
  )
}



export default NavBar;