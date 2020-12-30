import React, { Component } from 'react';
import { Container } from 'react-bootstrap';
class HomePage extends Component {

    render() {
      return (
        <Container className="page">
          <h2>Welcome to Liber Fabulas!</h2>
          <br></br>
          <p>If you want to upload a story, please create an account and sign in. Otherwise, feel free to browse the stories that have already been uploaded</p>
          <p>I'm afraid I haven't had time to optimize this website for mobile yet. Please switch to a computer for the full experience, or bear with us during this pre-alpha phase</p>
        </Container>
      )
    }
  }
  
  export default HomePage;