import React, { Component } from 'react';
import Navbar from 'react-bootstrap/Navbar';
import Nav from 'react-bootstrap/Nav';
import { LinkContainer } from 'react-router-bootstrap';
// 

class NavBar extends Component {
  render() {
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
            </Nav>
          </Navbar.Collapse>
        </Navbar>
      </header>
    )
  }
}

export default NavBar;