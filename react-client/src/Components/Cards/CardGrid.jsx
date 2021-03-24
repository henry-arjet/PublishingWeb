import React, { Component} from 'react';
import {Container} from 'react-bootstrap';
import StoryCard from './StoryCard'
import CategoryCard from './CategoryCard';
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

    TimeString(upTime){
        let epoch = Date.now();
        let mils = epoch-upTime; //how many miliseconds ago was it uploaded
        if (mils > 31536000000){
            let ret = Math.floor(mils/31536000000);
            if (ret == 1){return ret + " year"}
            return ret + " years"
        }
        else if (mils > 2592000000 ){
            let ret = Math.floor(mils/2592000000);
            if (ret == 1){return ret + " month"}
            return ret + " months"
        }
        else if (mils > 604800000 ){
            let ret = Math.floor(mils/604800000);
            if (ret == 1){return ret + " week"}
            return ret + " weeks"
        }
        
        else if (mils > 86400000 ){
            let ret = Math.floor(mils/86400000);
            if (ret == 1){return ret + " day"}
            return ret + " days"
        }
        else if (mils > 3600000 ){
            let ret = Math.floor(mils/3600000);
            if (ret == 1){return ret + " hour"}
            return ret + " hours"
        }
        else if (mils > 60000 ){
            let ret = Math.floor(mils/60000);
            if (ret == 1){return ret + " minute"}
            return ret + " minutes"
        }
        else{
            let ret = Math.floor(mils/1000);
            if (ret == 1){return ret + " second"}
            return ret + " seconds"
        }

    }

    MakeCards(props){//outputs a list of cards
        console.log("hit");
        if(this.props.categories){
            console.log("test1");
            return(this.MakeCats(this.props))
        }else {
            console.log("test2");
            return(this.MakeStories(this.props));
        }
    }
    MakeStories(props){//outputs a list of cards
        console.log("hitS");
        var cardList = [];
        for (var i = 0; i < props.results.length; i++){
            let li = "story/" + props.results[i].id;
            cardList.push(<StoryCard link={li} title={props.results[i].title}
                 rating={props.results[i].rating == 0?null:(props.results[i].rating / 10000).toFixed(1).toString()}//0 is default for unrated values
                 views={this.CleanViews(props.results[i].views)} 
                 time={this.TimeString(props.results[i].timestamp)}
                 authorID={props.results[i].authorID}
                 authorName={props.results[i].authorName}
                 words={props.results[i].wordcount}/>);
        }
        return cardList;
    }

    MakeCats(props){
        console.log("hitC");
        let cardList = [];
        for (let i = 0; i<props.categories.length; i++){
            let li = "categories/" + props.categories[i].linkName;
            cardList.push(<CategoryCard link={li} title={props.categories[i].name} description={props.categories[i].description}/>);
        }
        return cardList
    }

    render() {
        return (
        <Container fluid className="cardGrid">
            {this.MakeCards(this.props)}
        </Container>
        );
    }
}

export default CardGrid;