import React, { useContext } from 'react';
import Navbar from 'react-bootstrap/Navbar';
import Nav from 'react-bootstrap/Nav';
import { LinkContainer } from 'react-router-bootstrap';
import { AuthContext } from '../Context/Auth';


function NavBar() {
  let auth = useContext(AuthContext);
  
  function ProfileButton () {
    if(auth.authTokens.id != 0){ //check to see if we're logged in
      return(
        <LinkContainer to={"/users/" + auth.authTokens.id}><Nav.Link className="headerNavButton">
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
  function DevTools(){
    if(auth.authTokens.privilege > 2){ //check to see if we're allowed access to the dev tools
      return(
        <LinkContainer to={"/devtools"}><Nav.Link className="headerNavButton">
          <strong>Dev Tools</strong>
        </Nav.Link></LinkContainer>
      );  
    }else return null;
  }

  return (
    <header fluid>
      <Navbar bg="light"> 
        <LinkContainer to="/"><Navbar.Brand><strong>Liber Fabulas</strong></Navbar.Brand></LinkContainer>
        <Navbar.Collapse>
          <Nav className="headerNav">
          <LinkContainer to="/toprated"><Nav.Link className="headerNavButton">
            <strong>Top Rated</strong>
          </Nav.Link></LinkContainer>
          <LinkContainer to="/mostviewed"><Nav.Link className="headerNavButton">
            <strong>Most Viewed</strong>
          </Nav.Link></LinkContainer>
          <LinkContainer to="/newest"><Nav.Link className="headerNavButton">
            <strong>New</strong>
          </Nav.Link></LinkContainer>
          <LinkContainer to="/categories"><Nav.Link className="headerNavButton">
            <strong>Categories</strong>
          </Nav.Link></LinkContainer>
          {DevTools()}
          {ProfileButton()}
          </Nav>
        </Navbar.Collapse>
      </Navbar>
    </header>
  )
}



export default NavBar;