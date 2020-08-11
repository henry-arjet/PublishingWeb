import React, { Component } from 'react';
import Blurb from './Blurb';
class HomePage extends Component {

    render() {
      return (
        <Blurb big="Test Blurb" little="This is a test of the emergency broadcast system. This is only a test. If this had been an actual emergency, instructions would follow." />
      )
    }
  }
  
  export default HomePage;