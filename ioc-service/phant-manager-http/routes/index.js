exports.home = function(req, res) {
  res.render('home', {
    title: 'Internet of Chicken'
  });
};
