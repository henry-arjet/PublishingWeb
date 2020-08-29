import React, { Component } from 'react';
import Blurb from '../Blurb';
import { Container } from 'react-bootstrap';

class StoryPage extends Component {
    constructor(props){
        super(props);
        this.state = {
          results: null,
          gotResults: false,
        };
    }
    componentDidMount(){
        fetch(window.location.pathname + ".html").then(response => response.text()).then(data => this.setState({results: data, gotResults: true}));
    }
    outputResult(){
      console.log(this.state.results);
      return {__html: ('<h1>Remember: this is DANGEROUS</h1>' + this.state.results)};
    }

    render() {
      if (this.state.gotResults == true){
        return (
          <Container className="page" >
            <div dangerouslySetInnerHTML={this.outputResult()}/>
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