import React, { Component } from 'react';
import CardGrid from './Cards/CardGrid'

class TopRatedPage extends Component {
  constructor(props){
    super(props);
    this.state = {
      results: [],
      gotResults: false,
    };
    //this.state.results = "5";
    const promise = fetch(window.location.protocol + "//" + window.location.host + '/results/?o=top')
    .then(response => response.json()).then(data => this.setState({results: data, gotResults: true}));
  }
  render() {
    if(this.state.gotResults==true){
      return (
        <CardGrid results = {this.state.results} />    
      )
    } else return(
      <div>
        <br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
        <br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
        <br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
      </div>
    )
    
  }
}

export default TopRatedPage;