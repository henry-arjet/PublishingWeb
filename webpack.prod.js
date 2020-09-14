const path = require('path');
const DIST_DIR = path.join(__dirname, '/react-client/dist');
const { merge } = require('webpack-merge');

const common = require('./webpack.common.js');

module.exports = merge(common, {

  mode: 'production',
  output: {
    path: DIST_DIR,
    filename: '[name].[contentHash].cbundle.js',
    chunkFilename: '[name].bundle.js',
  },
});