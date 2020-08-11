import React, { Component } from 'react';
import { Container } from 'react-bootstrap';
class Blurb extends Component {
    constructor(props){
    super(props);
    }
    render() {
        return (
            <Container className="blurbBox">
                <h2>{this.props.big}</h2>
                {this.props.little}
            </Container>
        )
    }
}

export default Blurb;