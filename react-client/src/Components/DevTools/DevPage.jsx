import React, { Component, useContext } from 'react';
import Button from 'react-bootstrap/Button';
import { AuthContext } from "../Context/Auth";
import { Container, Row, Form, Col } from 'react-bootstrap';

function DevPage(){

    let auth = useContext(AuthContext);

    function handleSubmit(event){
        const form = event.currentTarget;
        event.preventDefault();
        event.stopPropagation();

        console.log("submitted");

        var ret = new Object();
        console.log(ret.type);
        ret.id = form.formID.value;
        form.formID.value = null;
        ret.title = form.formTitle.value;
        form.formTitle.value = null;
        ret.path = form.formPath.value;
        form.formPath.value = null;
        ret.rating = form.formRating.value;
        form.formRating.value = null;
        ret.views = form.formViews.value;
        form.formViews.value = null;
        ret.authorID = form.authorID.value;
        form.authorID.value = null;
        ret.permission = form.permission.value;
        form.permission.value = null;
        if(ret.id){ //If we specified an ID, aka if we need to update an existing entry
            fetch(window.location.origin + "/db/update/story", {
                method: "POST",
                headers: {'Content-Type': 'application/json',},
                body: JSON.stringify(ret),
            }).catch((error) => {
                console.error('Error:', error);
            });
        }else{ //If no ID is specified, we assume it's an insert
            fetch(window.location.origin + "/db/add/story", {
                method: "PUT",
                headers: {'Content-Type': 'application/json',
                    Authorization: "Basic " + btoa(auth.authTokens.id + ":" + auth.authTokens.password),},
                body: JSON.stringify(ret),
            }).catch((error) => {
                console.error('Error:', error);
            });
        }
    }
    function sortTop(){
        fetch(window.location.origin + "/db/resort/toprated", {method: 'POST',
        headers: { Authorization: "Basic " + btoa(auth.authTokens.id + ":" + auth.authTokens.password),},
        })
    }
    function sortViews(){
        fetch(window.location.origin + "/db/resort/mostviewed", {method: 'POST',
        headers: { Authorization: "Basic " + btoa(auth.authTokens.id + ":" + auth.authTokens.password),},
        })
    }
      return (
        <Container className="page">
            <Row className="devButtonRow">
                <Button variant="dark" onClick={sortTop} className="paddedButton">Re-Sort Top Rated</Button>
                <Button variant="dark" onClick={sortViews} className="paddedButton">Re-Sort Most Viewed</Button>
            </Row>
            <Row>
                <Form onSubmit={handleSubmit}>
                    <h2>Create or Update Database Entry</h2>
                    <Form.Group controlId="formID">
                        <Form.Label>ID Number</Form.Label>
                        <Form.Control type="text" placeholder="ID" />
                        <Form.Text className="text-muted">
                            Only fill to update an existing entry. Leave blank for a new entry.
                        </Form.Text>
                    </Form.Group>
                    <Form.Group controlId="formTitle">
                        <Form.Label>Searchable Title</Form.Label>
                        <Form.Control type="text" placeholder="Title" />
                    </Form.Group>
                    <Form.Group controlId="formPath">
                        <Form.Label>Path</Form.Label>
                        <Form.Control type="text" placeholder="Path" />
                    </Form.Group>
                    <Form.Group controlId="formRating">
                        <Form.Label>Rating</Form.Label>
                        <Form.Control type="text"  min="0" max="50000" placeholder="number/50,000" />
                        <Form.Text className="text-muted">
                            This is a whole number out of 50,000. It gets devided by 10,000 to become x/5
                        </Form.Text>
                    </Form.Group>
                    <Form.Group controlId="formViews">
                        <Form.Label>Number of Views</Form.Label>
                        <Form.Control type="text" min="0" placeholder="Views" />
                    </Form.Group>
                    <Form.Group controlId="authorID">
                        <Form.Label>Author ID</Form.Label>
                        <Form.Control type="text" min="0" placeholder="Author" />
                        <Form.Text className="text-muted">
                            The ID of the user to whom this story belongs
                        </Form.Text>
                    </Form.Group>
                    <Form.Group controlId="permission">
                        <Form.Label>Permission</Form.Label>
                        <Form.Control type="text" min="1" max="255" placeholder="Permission" />
                        <Form.Text className="text-muted">
                            Who can access. 1 is public, 2 is private, 3 is locked.
                        </Form.Text>
                    </Form.Group>
                    <Button variant="dark" type="submit">Submit form</Button>
                </Form>
            </Row>
        </Container>
    );
}
  
  export default DevPage;