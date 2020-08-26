import React, { Component } from 'react';
import Blurb from '../Blurb';
import { Container } from 'react-bootstrap';

class StoryPage extends Component {
    constructor(props){
        super(props);
        this.state = {
          results: [],
          gotResults: false,
        };
    }
    componentDidMount(){
        let idString = window.location.pathname.substring(window.location.pathname.lastIndexOf("/")+1);
        fetch(window.location.pathname + "?id=" + idString).then(response => response.json()).then(data => this.setState({results: data, gotResults: true}));
    }
    render() {
      if (this.state.gotResults == true){
        return (
          <Container className="page" >
            <h2>{this.state.results.text.fullTitle}</h2>
            <br/>
            <text className="jsonOutput">
              {this.state.results.text.chapter1}
            </text>
          </Container>
        )
      }
      return (
        <div>
        <br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
        <br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
        <br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
      </div>
      )
    }
  }
  
  export default StoryPage;