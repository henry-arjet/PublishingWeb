import React, { Component } from 'react';
import Blurb from '../Blurb';

class StoryPage extends Component {
    constructor(props){
        super(props);
        this.state = {
          results: [],
          gotResults: false,
        };
        //fetch(useLocation().pathname + "?id=" + idString).then(response => response.json()).then(data => this.setState({results: data, gotResults: true}));
    }
    componentDidMount(){
        let idString = window.location.pathname.substring(window.location.pathname.lastIndexOf("/")+1);
        fetch(window.location.pathname + "?id=" + idString).then(response => response.json()).then(data => this.setState({results: data, gotResults: true}));
    }
    render() {
      if (this.state.gotResults == true){
        return (
          <Blurb big={this.state.results.text.fullTitle} little={this.state.results.text.chapter1} />
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