import React, { Component} from 'react';
import {Container} from 'react-bootstrap';
import StoryCard from './StoryCard'
class CardGrid extends Component {
    constructor(props){
    super(props);
    this.state = {
        numCards: 0,
    }
    }//<h2>{this.props.results[0].title}</h2>

    CleanViews(views){
        if(views < 1000){
            return views.toString();
        }else if (views < 10000){
            return (views / 1000).toFixed(1).toString() + "K";
        }else if (views < 1000000){
            return (views/1000).toFixed(0).toString() + "K";
        }else if(views < 10000000) {
            return (views/1000000).toFixed(1).toString() + "M";
        }else return (views/1000000).toFixed(0).toString() + "M";
    }

    MakeCards(props){//outputs a list of cards
        var cardList = [];
        for (var i = 0; i < props.results.length; i++){
            let li = "story/" + props.results[i].id;
            cardList.push(<StoryCard link={li} title={props.results[i].title}
                 rating={(props.results[i].rating / 10000).toFixed(1).toString()}
                 views={this.CleanViews(props.results[i].views)} />);
        }
        return cardList;
    }

    render() {
        return (
        <Container fluid className="cardGrid">
            {this.MakeCards(this.props)}
        </Container>
        )
    }
}

export default CardGrid;