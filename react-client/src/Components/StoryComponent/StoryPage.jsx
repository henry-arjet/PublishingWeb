import React, { Component } from 'react';
import { Container } from 'react-bootstrap';

class StoryPage extends Component {
    constructor(props){
        super(props);
        this.state = {
          html: null,
          meta: null,
          gotResults: 0,
        };
    }
    componentDidMount(){
        fetch(window.location.pathname + "?t=t&id=" + window.location.pathname.substring(window.location.pathname.lastIndexOf("/") + 1))
          .then(response => response.text()).then(data => this.setState({html: data, gotResults: this.state.gotResults+1}));
        fetch(window.location.pathname + "?t=m&id=" + window.location.pathname.substring(window.location.pathname.lastIndexOf("/") + 1))
          .then(response => response.text()).then(data => this.setState({meta: data, gotResults: this.state.gotResults+1}));
    }
    outputResult(){
      console.log(this.state.html);
      return {__html: (this.state.html)};
    }

    render() {
      if (this.state.gotResults == 2){
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