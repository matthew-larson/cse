require 'fileutils'
require 'json'
require 'yaml'
require 'set'
require_relative 'lib/pandoc'
require_relative 'lib/tables'
require_relative 'lib/toc'

THIS_DIR = File.expand_path(File.dirname(__FILE__))
REFERENCE_DIR = File.expand_path(File.join("config", "reference"), THIS_DIR)
RECORD_INDEX_FILE = File.join(REFERENCE_DIR, 'record-index.yaml')
RECORD_NAME_FILE = File.join(REFERENCE_DIR, 'known-records.txt')
MANIFEST = [
  [1, "introduction.md"],
  [1, "about-cse.md"],
  [1, "operation.md"],
  [1, "input-structure.md"],
  [1, "input-data.md"],
  [2, "records/top-members.md"],
  [2, "records/holiday.md"],
  [2, "records/material.md"],
  [2, "records/construction.md"],
  [3, "records/layer.md"],
  [2, "records/glazetype.md"],
  [2, "records/meter.md"],
  [2, "records/zone.md"],
  [3, "records/gain.md"],
  [3, "records/surface.md"],
  [3, "records/window.md"],
  [3, "records/shade.md"],
  [3, "records/sgdist.md"],
  [3, "records/door.md"],
  [3, "records/perimeter.md"],
  [3, "records/terminal.md"],
  [2, "records/izxfer.md"],
  [2, "records/rsys.md"],
  [2, "records/ductseg.md"],
  [2, "records/dhwsys.md"],
  [2, "records/dhwheater.md"],
  [2, "records/dhwtank.md"],
  [2, "records/dhwpump.md"],
  [2, "records/dhwloop.md"],
  [2, "records/dhwlooppump.md"],
  [2, "records/dhwloopseg.md"],
  [2, "records/dhwloopbranch.md"],
  [2, "records/airhandler.md"],
  [2, "records/heatplant.md"],
  [3, "records/boiler.md"],
  [2, "records/coolplant.md"],
  [3, "records/chiller.md"],
  [2, "records/towerplant.md"],
  [2, "records/hploop.md"],
  [2, "records/reportfile.md"],
  [2, "records/report.md"],
  [2, "records/reportcol.md"],
  [2, "records/exportfile.md"],
  [2, "records/export.md"],
  [2, "records/exportcol.md"],
  [1, "output-reports.md"],
]
Levels = MANIFEST.map {|level, _| level}
Files = MANIFEST.map {|_, path| path}
PANDOC_GENERAL_OPTIONS = [
  "--parse-raw",
  "--standalone",
  "--wrap=none",
].join(' ')
PANDOC_MD_OPTIONS = PANDOC_GENERAL_OPTIONS + " " + [
  "--atx-headers",
  "--normalize",
  "--to markdown",
  "--from markdown",
].join(' ')
EnsureExists = lambda do |path|
  FileUtils.mkdir_p(path) unless File.exist?(path)
end
EnsureAllExist = lambda do |paths|
  paths.each {|p| EnsureExists[p]}
end
Run = lambda do |cmd, working_dir=nil|
  working_dir ||= Dir.pwd
  Dir.chdir(working_dir) do
    begin
      result = `#{cmd}`
      if !result or result.empty?
        puts("... success!")
      else
        puts("... success! result:\n#{result}")
      end
    rescue => e
      puts("... failure:\n#{e.message}")
    end
  end
end
ExpandPathsFrom = lambda do |config|
  reference_dir = config.fetch('reference-dir')
  lambda do |manifest|
    new_manifest = []
    manifest.each do |path|
      new_manifest << File.expand_path(path, reference_dir)
    end
    new_manifest
  end
end
NormalizeMarkdown = lambda do |config|
  out_dir = config.fetch("output-dir")
  EnsureExists[out_dir]
  lambda do |manifest|
    new_manifest = []
    manifest.each do |path|
      out_path = File.join(out_dir, File.basename(path))
      new_manifest << out_path
      if not FileUtils.uptodate?(out_path, [path])
        Run["pandoc #{PANDOC_MD_OPTIONS} -o #{out_path} #{path}"]
      end
    end
    new_manifest
  end
end
AdjustMarkdownLevels = lambda do |config|
  levels = config.fetch("levels")
  out_dir = config.fetch("output-dir")
  EnsureExists[out_dir]
  lambda do |manifest|
    new_manifest = []
    if manifest.length != levels.length
      puts("length of manifest doesn't equal length of levels:")
      puts("... manifest length: #{manifest.length}")
      puts("... levels length: #{levels.length}")
      exit(1)
    end
    manifest.zip(levels).each do |path, level|
      out_path = File.join(out_dir, File.basename(path))
      new_manifest << out_path
      if level == 1
        FileUtils.cp(path, out_path)
      else
        adjustment = "#" * (level - 1)
        md = File.read(path)
        File.open(out_path, 'w') do |f|
          md.lines.each do |line|
            if line =~ /^#+\s+/
              f.write(adjustment + line)
            else
              f.write(line)
            end
          end
        end
      end
    end
    new_manifest
  end
end
AddFiles = lambda do |config|
  new_paths = config.fetch("paths")
  out_dir = config.fetch("output-dir")
  add_to_manifest = config.fetch("append-to-manifest?", false)
  EnsureExists[out_dir]
  lambda do |manifest|
    new_manifest = []
    manifest.each do |path|
      out_path = File.join(out_dir, File.basename(path))
      new_manifest << out_path
      if !FileUtils.uptodate?(out_path, [path])
        FileUtils.cp(path, out_path)
      end
    end
    new_paths.each do |path|
      out_path = File.join(out_dir, File.basename(path))
      new_manifest << out_path if add_to_manifest
      if !FileUtils.uptodate?(out_path, [path])
        FileUtils.cp(path, out_path)
      end
    end
    new_manifest
  end
end
CopyToDir = lambda do |config|
  out_dir = config.fetch("output-dir")
  EnsureExists[out_dir]
  lambda do |manifest|
    new_manifest = []
    manifest.each do |path|
      out_path = File.join(out_dir, File.basename(path))
      new_manifest << path
      if !FileUtils.uptodate?(out_path, [path])
        FileUtils.cp(path, out_path)
      end
    end
    new_manifest
  end
end
CopyByGlob = lambda do |config|
  from_glob = config.fetch("from-glob")
  out_dir = config.fetch("output-dir")
  EnsureExists[out_dir]
  lambda do
    new_manifest = []
    Dir[from_glob].each do |path|
      out_path = File.join(tgt_dir, File.basename(path))
      new_manifest << out_path
      if not FileUtils.uptodate?(out_path, [path])
        FileUtils.cp(path, out_path)
      end
    end
    new_manifest
  end
end
PassThroughWithSideEffect = lambda do |config|
  fn = config.fetch("function")
  disable = config.fetch("disable?", false)
  lambda do |x|
    fn[] unless disable
    x
  end
end
UpdateOutlineCount = lambda do |count, level_at|
  new_count = []
  mod_idx = level_at - 1
  0.upto(mod_idx).each do |idx|
    c = count.fetch(idx, 0)
    if idx == mod_idx
      new_count << c+1
    elsif idx < mod_idx
      if idx > (count.length-1)
        new_count << c+1
      else
        new_count << c
      end
    else
      next
    end
  end
  new_count
end
OutlineCountToStr = lambda do |count|
  count.map(&:to_s).join(".")
end
NumberMd = lambda do |config|
  if config.fetch("disable", false)
    lambda do |manifest|
      manifest
    end
  else
    out_dir = config.fetch("output-dir")
    levels = config.fetch("levels", nil)
    starting_count = config.fetch("starting-count", [0])
    exceptions = config.fetch("exceptions", [])
    EnsureExists[out_dir]
    unnum = lambda do |line|
      line =~ /{-}\s*$/ || line =~ /{[^}]*?\.unnumbered[^}]*?}\s*$/
    end
    lambda do |manifest|
      new_manifest = []
      count = starting_count
      manifest.each_with_index do |path, idx|
        bn = File.basename(path)
        out_path = File.join(out_dir, bn)
        new_manifest << out_path
        if exceptions.include?(bn)
          if !FileUtils.uptodate?(out_path, [path])
            FileUtils.cp(path, out_path)
          end
          next
        end
        level_adjustment = levels ? (levels[idx] - 1) : 0
        md = File.read(path)
        if !FileUtils.uptodate?(out_path, [path])
          File.open(out_path, 'w') do |f|
            md.lines.each do |line|
              if line =~ /^#+\s+/ and !unnum[line]
                m = line.match(/^(#+)\s+(.*)$/)
                level_at = m[1].length + level_adjustment
                count = UpdateOutlineCount[count, level_at]
                outline_number = OutlineCountToStr[count]
                f.write(m[1] + ' ' + outline_number + ' ' + m[2].chomp + "\n")
              else
                f.write(line)
              end
            end
          end
        end
      end
      new_manifest
    end
  end
end
# String -> String
# Attempts to remove a plural "s" from a word if appropriate. Note: this is a
# very basic algorithm.
DePluralize = lambda do |word|
  if word.end_with?("s")
    if word.length > 1
      word[0..-2]
    end
  else
    word
  end
end
XLinkMarkdown = lambda do |config|
  disable = config.fetch('disable?', false)
  if disable
    lambda do |manifest|
      manifest
    end
  else
    oi = config.fetch('object-index')
    ons = config.fetch('object-name-set')
    out_dir = config.fetch('output-dir')
    EnsureExists[out_dir]
    lambda do |manifest|
      new_manifest = []
      manifest.each do |path|
        bn = File.basename(path)
        out_path = File.join(out_dir, bn)
        new_manifest << out_path
        next if FileUtils.uptodate?(out_path, [path])
        f = lambda do |tag, the_content, state|
          g = lambda do |inline|
            if inline['t'] == 'Str'
              index = state[:oi]
              ons = state[:ons]
              word = inline['c']
              word_only = word.scan(/[a-zA-Z:]/).join
              word_only_singular = DePluralize[word_only]
              if ons.include?(word_only) or ons.include?(word_only_singular)
                if index.include?(word_only) or index.include?(word_only_singular)
                  w = ons.include?(word_only) ? word_only : word_only_singular
                  state[:num_hits] += 1
                  ref = index[w].scan(/(\#.*)/).flatten[0]
                  {'t' => 'Link',
                   'c' => [
                     ["", [], []],
                     [{'t'=>'Str','c'=>word}],
                     [ref, '']]}
                else
                  puts("WARNING! ObjectNameSet includes #{word_only}|#{word_only_singular}; Index doesn't")
                  inline
                end
              else
                inline
              end
            elsif inline['t'] == 'Emph'
              new_ins = inline['c'].map(&g)
              {'t'=>'Emph', 'c'=>new_ins}
            elsif inline['t'] == 'Strong'
              new_ins = inline['c'].map(&g)
              {'t'=>'Strong', 'c'=>new_ins}
            else
              inline
            end
          end
          if tag == "Para"
            new_inlines = the_content.map(&g)
            {'t' => 'Para',
             'c' => new_inlines}
          else
            nil
          end
        end
        content = File.read(path)
        doc = JSON.parse(Pandoc::MdToJson[content])
        state = {
          num_hits: 0,
          oi: oi,
          ons: ons
        }
        new_doc = Pandoc::Walk[doc, f, state]
        new_content = Pandoc::JsonToMd[new_doc]
        puts("Crosslinked #{bn}; #{state[:num_hits]} found")
        File.write(out_path, new_content)
      end
      new_manifest
    end
  end
end
JoinManifestToString = lambda do |manifest|
  manifest.join(' ')
end
RunPandoc = lambda do |config|
  opts = config.fetch('options')
  out_path = config.fetch('output-path')
  out_dir = File.dirname(out_path)
  working_dir = config.fetch('working-dir', out_dir)
  EnsureExists[out_dir]
  lambda do |input|
    cmd = "pandoc #{opts} -o #{out_path} #{input}"
    Run[cmd, working_dir]
    out_path
  end
end
RunPandocOverEach = lambda do |config|
  opts = config.fetch('options')
  out_dir = config.fetch('output-dir')
  working_dir = config.fetch('working-dir', nil)
  do_nav = config.fetch('do-navigation?', false)
  top = config.fetch('top-url', nil)
  toc = config.fetch('toc-url', nil)
  EnsureExists[out_dir]
  lambda do |manifest|
    new_manifest = []
    num_files = manifest.length
    manifest.each_with_index do |path, idx|
      out_path = File.join(out_dir, File.basename(path, '.md') + '.html')
      new_manifest << out_path
      working_dir = File.dirname(path) if working_dir.nil?
      if !FileUtils.uptodate?(out_path, [path])
        nav_opts = []
        prev = File.basename(manifest[(idx-1)%num_files], '.md') + '.html'
        the_next = File.basename(manifest[(idx+1)%num_files], '.md') + '.html'
        nav_opts << "--variable prev=\"#{prev}\"" if num_files > 1
        nav_opts << "--variable next=\"#{the_next}\"" if num_files > 1
        nav_opts << "--variable top=\"#{top}\"" if top
        nav_opts << "--variable nav-toc=\"#{toc}\"" if toc
        nav_opts << "--variable do-nav=true" if do_nav
        all_opts = opts + ' ' + nav_opts.join(' ')
        cmd = "pandoc #{all_opts} -o #{out_path} #{path}"
        Run[cmd, working_dir]
      end
    end
    new_manifest
  end
end
GenTOC = lambda do |config|
  max_level = config.fetch('max-level')
  out_dir = config.fetch('output-dir')
  toc_name = config.fetch('toc-name', 'index.md')
  levels = config.fetch('levels')
  disable = config.fetch('disable?', false)
  if disable
    lambda do |manifest|
      manifest
    end
  else
    EnsureExists[out_dir]
    lambda do |manifest|
      new_manifest = []
      toc_out_path = File.join(out_dir, toc_name)
      new_manifest << toc_out_path
      if !FileUtils.uptodate?(toc_out_path, manifest)
        toc_content = TOC::GenTableOfContentsFromFiles[
          max_level - 1,
          levels.zip(manifest)
        ]
        File.write(toc_out_path, toc_content)
      end
      manifest.each do |path|
        out_path = File.join(out_dir, File.basename(path))
        new_manifest << out_path
        if !FileUtils.uptodate?(out_path, [path])
          FileUtils.cp(path, out_path)
        end
      end
      new_manifest
    end
  end
end
Listify = lambda {|x| [x]}
JoinFunctions = lambda do |fs|
  lambda do |x|
    fs.each do |f|
      x = f[x]
    end
    x
  end
end
Template = lambda do |config|
  lambda do |manifest|
    new_manifest = []
    # out_path = ...
    # FileUtils.uptodate?(out_path, [path])
    # ... do something ...
    # ... or copy ...
    new_manifest
  end
end
NewManifest = lambda do |manifest|
  lambda do |_|
    manifest
  end
end
CompressCSS = lambda do |config|
  disable = config.fetch("disable?", false)
  out_dir = config.fetch("output-dir")
  EnsureExists[out_dir]
  if disable
    lambda do |manifest|
      new_manifest = []
      manifest.each do |path|
        out_path = File.join(out_dir, File.basename(path))
        new_manifest << out_path
        if !FileUtils.uptodate?(out_path, [path])
          FileUtils.cp(path, out_path)
        end
      end
      new_manifest
    end
  else
    working_dir = config.fetch("working-dir", out_dir)
    cssnano = config.fetch("path-to-cssnano")
    lambda do |manifest|
      new_manifest = []
      manifest.each do |path|
        out_path = File.join(out_dir, File.basename(path))
        new_manifest << out_path
        if !FileUtils.uptodate?(out_path, [path])
          cmd = "#{cssnano} < #{path} > #{out_path}"
          Run[cmd, working_dir]
        end
      end
      new_manifest
    end
  end
end
CompressHTML = lambda do |config|
  out_dir = config.fetch("output-dir")
  disable = config.fetch("disable?", false)
  EnsureExists[out_dir]
  if disable
    lambda do |manifest|
      new_manifest = []
      manifest.each do |path|
        out_path = File.join(out_dir, File.basename(path))
        new_manifest << out_path
        if !FileUtils.uptodate?(out_path, [path])
          FileUtils.cp(path, out_path)
        end
      end
      new_manifest
    end
  else
    html_minifier = config.fetch("path-to-html-minifier")
    working_dir = config.fetch("working-dir", out_dir)
    opts = config.fetch("options", "")
    lambda do |manifest|
      new_manifest = []
      manifest.each do |path|
        out_path = File.join(out_dir, File.basename(path))
        new_manifest << out_path
        if !FileUtils.uptodate?(out_path, [path])
          if html_minifier
            cmd = "#{html_minifier} #{opts} -o #{out_path} #{path}"
            Run[cmd, working_dir]
          else
            FileUtils.cp(path, out_path)
          end
        end
      end
      new_manifest
    end
  end
end
BuildProbesAndCopyIntoManifest = lambda do |config|
  disable = config.fetch("disable?", false)
  if disable
    lambda do |manifest|
      manifest
    end
  else
    probes_dir = config.fetch('probes-build-dir')
    out_dir = config.fetch('output-dir')
    insert_after = config.fetch('insert-after-file')
    EnsureAllExist[[probes_dir, out_dir]]
    probes_input = config.fetch('path-to-probes-input')
    probes = YAML.load_file(probes_input)
    probes_md_path = File.join(probes_dir, 'probes.md')
    File.open(probes_md_path, 'w') do |f|
      f.write("# Probe Definitions\n\n")
      probes.keys.sort_by {|k| k.downcase}.each do |k|
        table = [["Name", "Input?", "Runtime?", "Type", "Variability", "Description"]]
        flds = probes[k][:fields]
        next if flds.empty?
        name = k
        array_txt = if probes[k][:array] then "[1..]" else "" end
        title = "\\@#{name}#{array_txt}."
        owner = probes[k][:owner]
        owner_txt = if owner == "--" then "" else " (owner: #{owner})" end
        f.write("## #{title}#{owner_txt}\n\n")
        if probes[k].include?(:description)
          f.write(probes[k][:description] + "\n\n")
        end
        flds.each do |fld|
          table << [
            fld[:name],
            if fld[:input] then "X" else "--" end,
              if fld[:runtime] then "X" else "--" end,
              fld[:type],
              fld[:variability],
              fld.fetch(:description, "--")
          ]
        end
        f.write(Tables::WriteTable[ table, true ])
        f.write("\n\n\n")
      end
    end
    lambda do |manifest|
      new_manifest = []
      manifest.each do |path|
        bn = File.basename(path)
        out_path = File.join(out_dir, bn)
        new_manifest << out_path
        if bn == insert_after
          probes_out_path = File.join(out_dir, File.basename(probes_md_path))
          new_manifest << probes_out_path
          if !FileUtils.uptodate?(probes_out_path, [probes_md_path])
            FileUtils.cp(probes_md_path, probes_out_path)
          end
        end
        if !FileUtils.uptodate?(out_path, [path])
          FileUtils.cp(path, out_path)
        end
      end
      new_manifest
    end
  end
end
ReLinkHTML = lambda do |config|
  out_dir = config.fetch("output-dir")
  tags_fname_map = config.fetch("tags-filename-map")
  EnsureExists[out_dir]
  lambda do |manifest|
    new_manifest = []
    m = manifest.length
    n = 0
    manifest.each do |path|
      bn = File.basename(path)
      out_path = File.join(out_dir, bn)
      new_manifest << out_path
      if !FileUtils.uptodate?(out_path, [path])
        n += 1
        content = File.read(path)
        new_content = tags_fname_map.to_a.inject(content) do |nc, tag_fname|
          tag = tag_fname[0]
          fname = tag_fname[1]
          if fname == bn
            nc
          else
            nc.gsub(/<a href="#{tag}">([^<]*)<\/a>/,
                    "<a href=\"#{fname}#{tag}\">\\1</a>")
          end
        end
        File.write(out_path, new_content)
      end
    end
    puts("Updated #{n}/#{m} files for relinking")
    new_manifest
  end
end
BuildSinglePageHTML = JoinFunctions[[
  ExpandPathsFrom[
    "reference-dir" => File.expand_path('src')
  ],
  NormalizeMarkdown[
    "output-dir" => File.join("build", "md", "normalize")
  ],
  XLinkMarkdown[
    "object-index" => YAML.load_file(RECORD_INDEX_FILE),
    "object-name-set" => Set.new(
      File.read(RECORD_NAME_FILE).lines.map {|x| x.strip}
    ),
    "output-dir" => File.expand_path(
      File.join("build", "md", "xlink"), THIS_DIR
    )
  ],
  AdjustMarkdownLevels[
    "output-dir" => File.join("build", "md", "adjusted-headers"),
    "levels" => Levels
  ],
  BuildProbesAndCopyIntoManifest[
    "probes-build-dir" => File.expand_path(
      File.join("build", "md", "probes"), THIS_DIR
    ),
    "output-dir" => File.expand_path(
      File.join("build", "md", "all-with-probes"), THIS_DIR
    ),
    "insert-after-file" => "output-reports.md",
    "path-to-probes-input" => File.expand_path(
      File.join("config", "reference", "probes_input.yaml"), THIS_DIR
    )
  ],
  PassThroughWithSideEffect[
    "function" => lambda do
      # Copy template to the right spot
      path = File.expand_path(
        File.join("config", "template", "site-template.html"), THIS_DIR
      )
      out_path = File.expand_path(
        File.join("build", "md", "all-with-probes", "site-template.html"), THIS_DIR
      )
      if !FileUtils.uptodate?(out_path, [path])
        FileUtils.cp(path, out_path)
      end
    end
  ],
  JoinManifestToString,
  RunPandoc[
    "options" => [
      "--parse-raw",
      "--standalone",
      "--wrap=none",
      "--to html5",
      "--from markdown",
      "--mathjax",
      "--number-sections",
      "--css=/css/base.css",
      "--table-of-contents",
      "--toc-depth=3",
      "--smart",
      "--variable do-nav=true",
      "--variable top=\"/index.html\"",
      "--template=site-template.html",
      "--variable title=\"CSE User's Manual\"",
      "--variable subtitle=\"California Simulation Engine\"",
      "--variable date=\"February 23, 2016\"",
    ].join(' '),
    "output-path" => File.expand_path(
      File.join("build", "html", "generated", "cse-user-manual.html"), THIS_DIR
    ),
    "working-dir" => File.expand_path(
      File.join("build", "md", "all-with-probes"), THIS_DIR
    ),
  ],
  Listify,
  CompressHTML[
    "path-to-html-minifier" => File.expand_path(
      File.join("node_modules", ".bin", "html-minifier"), THIS_DIR
    ),
    "options" => [
      "--minify-css",
      "--minify-js",
      "--remove-comments"
    ].join(' '),
    "output-dir" => File.expand_path(
      File.join("build", "output"), THIS_DIR
    )
  ],
  NewManifest[
    Dir[
      File.expand_path(File.join("config", "css", "*.css"), THIS_DIR)
    ]
  ],
  CompressCSS[
    "path-to-cssnano" => File.expand_path(
      File.join("node_modules", ".bin", "cssnano"), THIS_DIR
    ),
    "output-dir" => File.expand_path(
      File.join("build", "output", "css"), THIS_DIR
    )
  ],
  NewManifest[
    Dir[File.expand_path(File.join("src", "media", "*"), THIS_DIR)]
  ],
  CopyToDir[
    "output-dir" => File.expand_path(
      File.join("build", "output", "media"), THIS_DIR
    )
  ],
]]
BuildMultiPageHTML = lambda do |config|
  tag = config.fetch("tag")
  build_dir = config.fetch("build-dir", "build")
  md_dir = config.fetch("md-dir", "md")
  html_dir = config.fetch("html-dir", "html")
  this_dir = config.fetch("this-dir", THIS_DIR)
  out_dir = config.fetch("output-dir", "build/output")
  record_index_file = config.fetch("record-index-file", RECORD_INDEX_FILE)
  record_name_file = config.fetch("record-name-file", RECORD_NAME_FILE)
  disable_probes = config.fetch("disable-probes?", false)
  disable_toc = config.fetch("disable-toc?", false)
  disable_xlink = config.fetch("disable-xlink?", false)
  disable_compression = config.fetch("disable-compression?", true)
  disable_numbering = config.fetch("disable-numbering?", false)
  do_navigation = config.fetch("do-navigation?", false)
  title = config.fetch("title", "CSE User's Manual")
  subtitle = config.fetch("subtitle", "California Simulation Engine")
  date = config.fetch("date", "February 23, 2016")
  levels = config.fetch("levels", Levels)
  JoinFunctions[[
    ExpandPathsFrom[
      "reference-dir" => File.expand_path('src')
    ],
    NormalizeMarkdown[
      "output-dir" => File.expand_path(
        File.join(build_dir, tag, md_dir, "normalize"), this_dir
      )
    ],
    XLinkMarkdown[
      "object-index" => YAML.load_file(record_index_file),
      "object-name-set" => Set.new(
        File.read(record_name_file).lines.map {|x|x.strip}
      ),
      "output-dir" => File.join(build_dir, tag, md_dir, "xlink"),
      "disable?" => disable_xlink
    ],
    BuildProbesAndCopyIntoManifest[
      "probes-build-dir" => File.expand_path(
        File.join(build_dir, tag, md_dir, "probes"), this_dir
      ),
      "output-dir" => File.expand_path(
        File.join(build_dir, tag, md_dir, "all-with-probes"), this_dir
      ),
      "insert-after-file" => "output-reports.md",
      "path-to-probes-input" => File.expand_path(
        File.join("config", "reference", "probes_input.yaml"), this_dir
      ),
      "disable?" => disable_probes
    ],
    NumberMd[
      "output-dir" => File.join(build_dir, tag, md_dir, "number"),
      "levels" => levels + (disable_probes ? [] : [1]),
      "disable?" => disable_numbering
    ],
    GenTOC[
      "max-level" => 3,
      "output-dir" => File.expand_path(
        File.join(build_dir, tag, md_dir, "toc"), this_dir
      ),
      "toc-name" => "index.md",
      "levels" => (levels + (disable_probes ? [] : [1])).map do |lev|
        lev - 1
      end,
      "disable?" => disable_toc
    ],
    AddFiles[
      "paths" => [File.expand_path(
        File.join("config", "template", "site-template.html"), this_dir
      )],
      "output-dir" => File.expand_path(
        File.join(build_dir, tag, md_dir, "ready-to-build"), this_dir
      )
    ],
    RunPandocOverEach[
      "options" => [
        "--parse-raw",
        "--standalone",
        "--wrap=none",
        "--to html5",
        "--from markdown",
        "--mathjax",
        "--css=css/base.css",
        "--smart",
        "--template=site-template.html",
        title ? "--variable title=\"#{title}\"" : "",
        subtitle ? "--variable subtitle=\"#{subtitle}\"" : "",
        date ? "--variable date=\"#{date}\"" : "",
      ].join(' '),
      "output-dir" => File.expand_path(
        File.join(build_dir, tag, html_dir, "generated-multipage"), this_dir
      ),
      "top-url" => "/index.html",
      "toc-url" => "index.html",
      "do-navigation?" => do_navigation
    ],
    ReLinkHTML[
      "tags-filename-map" => lambda do
        index = YAML.load_file(RECORD_INDEX_FILE)
        index.to_a.sort_by do |e|
          e[0]
        end.inject({}) do |m, e|
          fname_tag = e[1].scan(/([^\#]*)(\#.*)/).flatten
          m.merge(Hash[fname_tag[1], fname_tag[0]])
        end
      end.call,
      "output-dir" => File.expand_path(
        File.join(build_dir, tag, html_dir, "relinked"), this_dir
      )
    ],
    CompressHTML[
      "path-to-html-minifier" => File.expand_path(
        File.join("node_modules", ".bin", "html-minifier"), this_dir
      ),
      "options" => [
        "--minify-css",
        "--minify-js",
        "--remove-comments"
      ].join(' '),
      "output-dir" => File.expand_path(out_dir, this_dir),
      "disable?" => disable_compression
    ],
    NewManifest[
      Dir[
        File.expand_path(File.join("config", "css", "*.css"), this_dir)
      ]
    ],
    CompressCSS[
      "path-to-cssnano" => File.expand_path(
        File.join("node_modules", ".bin", "cssnano"), this_dir
      ),
      "output-dir" => File.expand_path(
        File.join(out_dir, "css"), this_dir
      ),
      "disable?" => disable_compression
    ],
    NewManifest[
      Dir[File.expand_path(File.join("src", "media", "*"), this_dir)]
    ],
    CopyToDir[
      "output-dir" => File.expand_path(
        File.join(out_dir, "media"), this_dir
      )
    ],
  ]]
end
BuildPDF = JoinFunctions[[
  ExpandPathsFrom[
    "reference-dir" => File.expand_path('src')
  ],
  NormalizeMarkdown[
    "output-dir" => File.expand_path(
      File.join("build", "md", "normalize"), THIS_DIR
    )
  ],
  XLinkMarkdown[
    "object-index" => YAML.load_file(RECORD_INDEX_FILE),
    "object-name-set" => Set.new(
      File.read(RECORD_NAME_FILE).lines.map {|x|x.strip}
    ),
    "output-dir" => File.join("build", "md", "xlink")
  ],
  AdjustMarkdownLevels[
    "output-dir" => File.expand_path(
      File.join("build", "md", "adjusted-headers"), THIS_DIR
    ),
    "levels" => Levels
  ],
  BuildProbesAndCopyIntoManifest[
    "probes-build-dir" => File.expand_path(
      File.join("build", "md", "probes"), THIS_DIR
    ),
    "output-dir" => File.expand_path(
      File.join("build", "md", "all-with-probes"), THIS_DIR
    ),
    "insert-after-file" => "output-reports.md",
    "path-to-probes-input" => File.expand_path(
      File.join("config", "reference", "probes_input.yaml"), THIS_DIR
    )
  ],
  PassThroughWithSideEffect[
    "function" => lambda do
      # Copy template to the right spot
      paths = [File.expand_path(
        File.join("config", "template", "template.tex"), THIS_DIR
      )]
      CopyToDir[
        "output-dir" => File.expand_path(
          File.join("build", "md", "all-with-probes"), THIS_DIR
        )
      ][paths]
    end
  ],
  PassThroughWithSideEffect[
    "function" => lambda do
      media_files = Dir[
        File.expand_path(File.join("src", "media", "*"), THIS_DIR)
      ]
      CopyToDir[
        "output-dir" => File.expand_path(
          File.join("build", "md", "all-with-probes", "media"), THIS_DIR
        )
      ][media_files]
    end
  ],
  JoinManifestToString,
  RunPandoc[
    "options" => [
      "--parse-raw",
      "--standalone",
      "--wrap=none",
      '--variable geometry="margin=1in"',
      '--variable urlcolor=cyan',
      "--latex-engine=xelatex",
      "--table-of-contents",
      "--toc-depth=3",
      "--number-sections",
      "--smart",
      "--template=template.tex",
      "--listings",
      "--from markdown",
      "--variable title=\"CSE User's Manual\"",
      "--variable subtitle=\"California Simulation Engine\"",
      "--variable date=\"February 23, 2016\"",
    ].join(' '),
    "output-path" => File.expand_path(
      File.join("build", "output", "pdfs", "cse-user-manual.pdf"), THIS_DIR
    ),
    "working-dir" => File.expand_path(
      File.join("build", "md", "all-with-probes"), THIS_DIR
    ),
  ],
]]
Test = lambda do |expr1, expr2|
  puts("Test: #{expr1} =? #{expr2}")
  puts("... expr1: #{eval(expr1).inspect}")
  puts("... expr2: #{eval(expr2).inspect}")
  puts("... ? #{eval(expr1) == eval(expr2)}")
end

########################################
# Examples
########################################

if false
  ExpandPathsFrom['reference-dir' => File.expand_path('src')][Files]
end

if false
  Test["UpdateOutlineCount[[0], 1]", "[1]"]
  Test["UpdateOutlineCount[[1,1,3], 2]", "[1,2]"]
  Test["UpdateOutlineCount[[1,1,3], 3]", "[1,1,4]"]
  Test["UpdateOutlineCount[[1,1,3], 1]", "[2]"]
  Test["UpdateOutlineCount[[1,1,3], 4]", "[1,1,3,1]"]
  Test["UpdateOutlineCount[[1,1,3], 5]", "[1,1,3,1,1]"]
  Test["OutlineCountToStr[[1,1,3]]", "\"1.1.3\""]
end

if true
  if false
    puts("#"*60)
    puts("Build Single-Page HTML")
    BuildSinglePageHTML[Files]
    puts("Single-Page HTML DONE!")
    puts("^"*60)
  end

  if false
    puts("#"*60)
    puts("Build Multi-Page HTML")
    BuildMultiPageHTML[Files]
    puts("Multi-Page HTML DONE!")
    puts("^"*60)
  end

  if false
    puts("#"*60)
    puts("Build PDF")
    BuildPDF[Files]
    puts("PDF DONE!")
    puts("^"*60)
  end

  if true 
    WebManifest = [
      [1, "index.md"]
    ]
    WebLevels = WebManifest.map {|x| x[0]}
    WebFiles = WebManifest.map {|x| x[1]}
    puts("#"*60)
    puts("Build Site HTML")
    BuildMultiPageHTML[
      "tag" => "web-site",
      "date" => nil,
      "subtitle" => nil,
      "title" => "California Simulation Engine",
      "do-navigation?" => false,
      "output-dir" => "build/output",
      "disable-toc?" => true,
      "disable-xlink?" => true,
      "levels" => WebLevels
    ][WebFiles]
    puts("Site HTML DONE!")
    puts("^"*60)
  end
end