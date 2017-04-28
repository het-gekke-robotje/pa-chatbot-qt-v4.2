using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HtmlAgilityPack;

namespace ProxyScraper
{
    public class Scraper
    {
        private HashSet<string> urls;
        public Scraper()
        {
            urls = new HashSet<string>();
            for (DateTime date = DateTime.Now.Subtract(TimeSpan.FromDays(60)); date.Date <= DateTime.Now; date = date.AddDays(1))
            {
                string url = "http://www.***************.net/search?updated-max=" + date.Year.ToString() + "-" + date.Month.ToString("D2") + "-" + date.Day.ToString("D2") + "T21:06:00%2B06:00&max-results=2";
                var Webget = new HtmlWeb();
                var doc = Webget.Load(url);
                foreach (HtmlNode link in doc.DocumentNode.SelectNodes("//a[@href]"))
                {
                    string href_url = link.Attributes["href"].Value;
                    if (href_url.StartsWith("http://www.***************.net/" + date.Year.ToString() + "/" + date.Month.ToString("D2") + "/" + date.Day.ToString("D2") + "-" + date.Month.ToString("D2") + "-" + (date.Year - 2000).ToString("D2")))
                    {
                        href_url = href_url.Replace("#more", "").Replace("#comment-form", "");
                        if (urls.Add(href_url))
                        {
                            var Webget_iplist = new HtmlWeb();
                            var doc_iplist = Webget.Load(href_url);

                            foreach (HtmlNode text in doc_iplist.DocumentNode.SelectNodes("//textarea[@wrap]"))
                            {
                                if (text.InnerText.Length != 0)
                                {
                                    Console.WriteLine(text.InnerText);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
