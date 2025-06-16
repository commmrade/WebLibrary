# axum

`weblib` is a C++ framework that lets you make web applications

[![Documentation](https://en.wikipedia.org/wiki/Subject%E2%80%93verb%E2%80%93object_word_order)][docs]

More information about this crate can be found in the documentation, that I didn't make.

## High level features

- Route requests to handlers.
- Simple and predictable error handling model.
- Write code with mininal boilerplate.
- Take full advantage of C++23 features. (I mean the lib doesn't really need C++23, but it looks cool and I like `ranges` library)

In particular the last point is what sets `axum` apart from other frameworks.
`axum` doesn't have its own middleware system but instead uses
[`tower::Service`]. This means `axum` gets timeouts, tracing, compression,
authorization, and more, for free. It also enables you to share middleware with
applications written using [`hyper`] or [`tonic`].

## ⚠ Breaking changes ⚠

We are currently working towards axum 0.9 so the `main` branch contains breaking
changes. See the [`0.8.x`] branch for what's released to crates.io.

[`0.8.x`]: https://github.com/tokio-rs/axum/tree/v0.8.x

## Usage example

```rust
use axum::{
    routing::{get, post},
    http::StatusCode,
    Json, Router,
};
use serde::{Deserialize, Serialize};

#[tokio::main]
async fn main() {
    // initialize tracing
    tracing_subscriber::fmt::init();

    // build our application with a route
    let app = Router::new()
        // `GET /` goes to `root`
        .route("/", get(root))
        // `POST /users` goes to `create_user`
        .route("/users", post(create_user));

    // run our app with hyper, listening globally on port 3000
    let listener = tokio::net::TcpListener::bind("0.0.0.0:3000").await.unwrap();
    axum::serve(listener, app).await.unwrap();
}

// basic handler that responds with a static string
async fn root() -> &'static str {
    "Hello, World!"
}

async fn create_user(
    // this argument tells axum to parse the request body
    // as JSON into a `CreateUser` type
    Json(payload): Json<CreateUser>,
) -> (StatusCode, Json<User>) {
    // insert your application logic here
    let user = User {
        id: 1337,
        username: payload.username,
    };

    // this will be converted into a JSON response
    // with a status code of `201 Created`
    (StatusCode::CREATED, Json(user))
}

// the input to our `create_user` handler
#[derive(Deserialize)]
struct CreateUser {
    username: String,
}

// the output to our `create_user` handler
#[derive(Serialize)]
struct User {
    id: u64,
    username: String,
}
```

You can find this [example][readme-example] as well as other example projects in
the [example directory][examples].

See the [crate documentation][docs] for way more examples.

## Performance

`axum` is a relatively thin layer on top of [`hyper`] and adds very little
overhead. So `axum`'s performance is comparable to [`hyper`]. You can find
benchmarks [here](https://github.com/programatik29/rust-web-benchmarks) and
[here](https://web-frameworks-benchmark.netlify.app/result?l=rust).

## Safety

This crate uses `#![forbid(unsafe_code)]` to ensure everything is implemented in
100% safe Rust.

## Minimum supported Rust version

axum's MSRV is 1.75.

## Examples

The [examples] folder contains various examples of how to use `axum`. The
[docs] also provide lots of code snippets and examples. For full-fledged examples, check out community-maintained [showcases] or [tutorials].

## Getting Help

In the `axum`'s repo we also have a [number of examples][examples] showing how
to put everything together. Community-maintained [showcases] and [tutorials] also demonstrate how to use `axum` for real-world applications. You're also welcome to ask in the [Discord channel][chat] or open a [discussion] with your question.

## Community projects

See [here][ecosystem] for a list of community maintained crates and projects
built with `axum`.

## Contributing

🎈 Thanks for your help improving the project! We are so happy to have
you! We have a [contributing guide][contributing] to help you get involved in the
`axum` project.

## License

This project is licensed under the [MIT license][license].

### Contribution

Unless you explicitly state otherwise, any contribution intentionally submitted
for inclusion in `axum` by you, shall be licensed as MIT, without any
additional terms or conditions.

[readme-example]: https://github.com/tokio-rs/axum/tree/main/examples/readme
[examples]: https://github.com/tokio-rs/axum/tree/main/examples
[docs]: https://docs.rs/axum
[`tower`]: https://crates.io/crates/tower
[`hyper`]: https://crates.io/crates/hyper
[`tower-http`]: https://crates.io/crates/tower-http
[`tonic`]: https://crates.io/crates/tonic
[contributing]: https://github.com/tokio-rs/axum/blob/main/CONTRIBUTING.md
[chat]: https://discord.gg/tokio
[discussion]: https://github.com/tokio-rs/axum/discussions/new?category=q-a
[`tower::Service`]: https://docs.rs/tower/latest/tower/trait.Service.html
[ecosystem]: https://github.com/tokio-rs/axum/blob/main/ECOSYSTEM.md
[showcases]: https://github.com/tokio-rs/axum/blob/main/ECOSYSTEM.md#project-showcase
[tutorials]: https://github.com/tokio-rs/axum/blob/main/ECOSYSTEM.md#tutorials
[license]: https://github.com/tokio-rs/axum/blob/main/axum/LICENSE



<!-- # SomeCoolName
This is just a library made for learning purposes. I don't think it is production-ready and will ever be


# server directory
- Cookie.hpp - managing cookies in both requests and responses
- CustomController, CustomFilter-2 - examples of custom stuff
- HttpController - base class for custom controllers
- HttpFilter - base class for custom filters
- HttpFramework - an abstraction class for calling HttpServer::instance and some more stuff 
- HttpHandle - storing an actual endpoint (the method, filters, allowed methods and etc..)
- HttpBinder - registering endpoints, filters
- HttpRequest - responsible for incoming requests (client to server)
- HttpResponse - responsible for outgoing requests (server to client)
- HttpResController - controller that sets up endpoints for resource requests (static files (located in PROJECT_DIR/static/\[public|private\])).
/private and /public are needed so that you can have both public resources and private (by default they are all public, but you can apply a filter)
- HttpRouter - responsible for managing incoming requests (routing to the correct endpoint, checking if it's a file request or not) 
- HttpServer - responsible for starting server, accepting requests
- Utils - different string utils
- Query - query for HttpRequest


# requests
- i put all my efforts into making "server" functionality, so requests part of the library is really basic and I don't really wanna rewrite it -->