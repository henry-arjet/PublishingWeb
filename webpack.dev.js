const { merge } = require('webpack-merge');
const path = require('path');
const DIST_DIR = path.join(__dirname, '/react-client/dist');


const common = require('./webpack.common.js');

module.exports = merge(common, {

    mode: 'development',
    output: {
      path: DIST_DIR,
      filename: '[name].bundle.js',
      chunkFilename: '[name].bundle.js',
    },
});